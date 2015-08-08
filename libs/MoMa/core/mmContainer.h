#ifndef __mmContainer__
#define __mmContainer__

#include <stdio.h>
#include <armadillo>

#ifdef WIN32 
#define round(a) (a < 0.0 ? ceil(a - 0.5) : floor(a + 0.5))
#endif
namespace MoMa {
    
    
    /**
     * A Class that provides a way to store one double value for each frame.
     * The timedflag precises if the framerate is fixed or if we use a timestamp
     * vector to define the occuring time for each frame.
     *
     */
    
    enum interpTypes{//TODO implement qslerp
        linear,
        qslerp
    };
    
    //TODO implement conversion from timestamped mode to fixedframerate (with linear (arma::interp1) and qslerp interpolation
    
    class timedContainer{
    public:
        timedContainer(){timedFlag=false;frameRate=1.f;};
        
        inline const arma::vec &getTimeStamps(){return timeStamp;};
        inline arma::vec &getRefTimeStamps(){return timeStamp;};
        inline bool isTimestamped(){return timedFlag;};
        bool setFrameRate(float pFrameRate){
            if (timedFlag)
                return false;
            else {
                frameRate=pFrameRate;
                return true;
            }
        }
        float getFrameRate(){
            return (timedFlag?-1.f:frameRate);
        }
        
    protected:
        bool timeStampIsValid(arma::vec pTime);
        void interpIndexFind(const arma::vec pVec, double pValue, unsigned int &index1, double &weight1, unsigned int &index2, double &weight2 );
        bool timedFlag;
        float frameRate;
        arma::vec timeStamp;
    };
    
    class timed1dContainer: public timedContainer{
    public:
        bool pushFrame(double pData){
            if (timedFlag)
                return false;
            data.insert_rows(data.n_elem, 1);
            data(data.n_elem-1)=pData;
            return true;
        };

        bool pushTimedFrame(double pData,double pTime);
        void pop_front(){
            data.shed_col(0);
            if (timedFlag){
                timeStamp.shed_row(0);
            }
        }
        double getTimedFrame(double pTime);
        inline double getIndexedFrame(unsigned int pIndex){return data(pIndex);};
        bool setTimedFrame(double pData,double pTime);
        bool setIndexedFrame(double pData,unsigned int pIndex);
        
        inline const arma::vec &getData(){return data;};
        inline arma::vec &getRefData(){return data;};
        
        bool setData(float pFrameRate, arma::vec pData);
        bool setTimedData(arma::vec pTime, arma::vec pData);
        inline unsigned int nOfFrames(){return data.n_elem;};
        inline unsigned int nOfNodes(){return 1;};
        void clear(){data.clear();};
    protected:
        arma::vec data;
        
    };
    
    class timed2dContainer: public timedContainer{
    public:
        timed2dContainer(){timedFlag=false;frameRate=1.f;}
        bool pushFrame(arma::vec pData){
            if (timedFlag)
                return false;
            if (data.n_elem>0&&pData.n_rows!=data.n_rows)
                return false;
            data.insert_cols(data.n_cols, pData);
                return true;
        };
        
        bool pushTimedFrame(arma::vec pData,double pTime);
        void pop_front(){
            data.shed_col(0);
            if (timedFlag){
                timeStamp.shed_row(0);
            }
        }
        
        arma::vec getTimedFrame(double pTime);
        inline arma::vec getIndexedFrame(unsigned int pIndex){return data.col(pIndex);};
        
        bool setTimedFrame(arma::vec pData,double pTime);
        bool setIndexedFrame(arma::vec pData,unsigned int pIndex);
        
        inline const arma::mat &getData(){return data;};
        inline arma::mat &getRefData(){return data;};
        
        bool setData(float pFrameRate, arma::mat pData);
        bool setTimedData(arma::vec pTime, arma::mat pData);
        bool setInterpAlgo(interpTypes interpAlgo){this->interpolAlgo=interpAlgo; return true;};
        inline unsigned int nOfFrames(){return data.n_cols;};
        inline unsigned int nOfNodes(){return 1;};
        void clear(){data.clear();};
    protected:
        interpTypes interpolAlgo;
        
        bool checkInput(arma::vec pData);
            
        arma::mat data;
        
    };
    class timed3dContainer: public timedContainer{
    public:
        timed3dContainer(){interpolAlgo=linear;};
        bool pushFrame(arma::mat pData){
            if (data.n_elem>0&&(pData.n_rows!=data.n_rows||pData.n_cols!=data.n_cols))
                return false;
            if (data.n_elem==0)
                data.resize(pData.n_rows, pData.n_cols, 1);
            else
                data.insert_slices(data.n_slices, 1);
            data.slice(data.n_slices-1)=pData;
            return true;
        };

        bool pushTimedFrame(arma::mat pData,double pTime);
        void pop_front(){
            data.shed_slice(0);
            if (timedFlag){
                timeStamp.shed_row(0);
            }
        }
        arma::mat getTimedFrame(double pTime);
        inline arma::mat getIndexedFrame(unsigned int pIndex){return data.slice(pIndex);};
        bool setTimedFrame(arma::mat pData,double pTime);
        bool setIndexedFrame(arma::mat pData,unsigned int pIndex);
        
        inline const arma::cube &getData(){return data;};
        inline arma::cube &getRefData(){return data;};
        
        bool setData(float pFrameRate, arma::cube pData);
        bool setTimedData(arma::vec pTime, arma::cube pData);
        bool setInterpAlgo(interpTypes interpAlgo){this->interpolAlgo=interpAlgo; return true;};
        inline unsigned int nOfFrames(){return data.n_slices;};
        inline unsigned int nOfNodes(){return data.n_cols;};
        void clear(){data.clear();};
    protected:
        
        interpTypes interpolAlgo;
        bool checkInput(arma::mat pData);
            
        arma::cube data;
        
    };
};

#endif /* defined(__MoMaBrouillon__mmContainer__) */

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
     */
    
    
    enum interpTypes {
        
        step, // Step interpolation
        linear, // Linear interpolation
        qslerp // Qslerp interpolation
        
        // TODO Implement 'qslerp'
    };
    
    // TODO implement conversion from timestamped mode to fixedframerate (with linear (arma::interp1) and qslerp interpolation
    
    // - Timed container -
    
    class timedContainer {
        
      public:
        
        timedContainer( void ) { timedFlag = false; frameRate = 1.0f; }
        
        inline const arma::vec &getTimeStamps( void ){ return( timeStamp ); }
        inline arma::vec &getRefTimeStamps( void ){ return( timeStamp ); }
        inline bool isTimestamped( void ){ return( timedFlag ); }
        
        inline double getFrameTime( unsigned int index );
        inline bool setFrameRate( float pFrameRate );
        inline float getFrameRate( void );
        
      protected:
        
        bool timeStampIsValid( arma::vec pTime );
        
        void interpIndexFind( const arma::vec pVec, double pValue, unsigned int &index1,
        double &weight1, unsigned int &index2, double &weight2 ); // Interpolation
        
        arma::vec timeStamp;
        float frameRate;
        bool timedFlag;
    };
    
    // - Inlined functions -
    
    double timedContainer::getFrameTime( unsigned int index ) {
        
        return( timedFlag?timeStamp[index]:frameRate*index );
    }
    
    bool timedContainer::setFrameRate( float pFrameRate ) {
        
        if( timedFlag ) {
            
            return false;
            
        } else {
            
            frameRate = pFrameRate;
            return true;
        }
    }
    
    float timedContainer::getFrameRate( void ) {
        
        return( timedFlag?-1.f:frameRate );
    }
    
    // - 1D timed container -
    
    class timed1dContainer: public timedContainer {
        
      public:
        
        // - Push/pop methods -
        
        inline bool pushFrame( double pData );
        bool pushTimedFrame( double pData, double pTime );
        inline void pop_front( void );
        
        // - Getters -
        
        inline double getIndexedFrame( unsigned int pIndex ) { return data( pIndex ); }
        double getTimedFrame( double pTime ); // Extract timely frame from container
        
        inline timed1dContainer chop( int pBegIndex, int pEndIndex ); // Chop container @ index
        inline timed1dContainer chop( double pBegTime, double pEndTime ); // TODO Chop container @ time
        
        // - Setters -
        
        bool setIndexedFrame( double pData, unsigned int pIndex );
        bool setTimedFrame( double pData, double pTime );
        
        inline const arma::vec &getData( void ) { return data; }
        inline arma::vec &getRefData( void ) { return data; }
        
        bool setData( float pFrameRate, arma::vec pData );
        bool setTimedData( arma::vec pTime, arma::vec pData );
        
        // - Misc -
        
        inline unsigned int nOfFrames( void ) { return data.n_elem; }
        inline unsigned int nOfNodes( void ) { return 1; }
        void clear( void ) { data.clear(); }
        
      protected:
        
        arma::vec data;
    };
    
    // - Inlined functions -
    
    bool timed1dContainer::pushFrame( double pData ) {
        
        if( timedFlag ) return false;
        
        data.insert_rows( data.n_elem, 1 );
        data( data.n_elem-1 ) = pData;
        
        return true;
    };
    
    void timed1dContainer::pop_front( void ) {
        
        data.shed_col( 0 ); // Shed one value
        if( timedFlag ) timeStamp.shed_row( 0 );
    }
    
    inline timed1dContainer timed1dContainer::chop( int pBegIndex, int pEndIndex ) {
    
        timed1dContainer oneVec = *this;
        
        oneVec.data = data.subvec( pBegIndex, pEndIndex ); // Chopping the data vector and timestamps (if any)
        if( isTimestamped() ) oneVec.getRefTimeStamps() = getRefTimeStamps().subvec( pBegIndex, pEndIndex );
        
        return( oneVec );
    }
    
    // - 2D timed container -
    
    class timed2dContainer: public timedContainer {
        
      public:
        
        timed2dContainer( void ) { timedFlag = false; frameRate = 1.0f; }
        
        // - Push/pop methods -
        
        inline bool pushFrame( arma::vec pData );
        bool pushTimedFrame( arma::vec pData, double pTime );
        inline void pop_front( void );
        
        // - Getters -
        
        inline arma::vec getIndexedFrame( unsigned int pIndex ){ return data.col( pIndex ); }
        arma::vec getTimedFrame( double pTime ); // Extract timely frame from container
        
        inline timed2dContainer chop( int pBegIndex, int pEndIndex ); // Chop container @ index
        inline timed2dContainer chop( double pBegTime, double pEndTime ); // TODO Chop container @ time
        
        // - Setters -
        
        bool setIndexedFrame( arma::vec pData, unsigned int pIndex );
        bool setTimedFrame( arma::vec pData, double pTime );
        
        inline const arma::mat &getData( void ) { return data; }
        inline arma::mat &getRefData( void ) { return data; }
        
        bool setData( float pFrameRate, arma::mat pData );
        bool setTimedData( arma::vec pTime, arma::mat pData );
        
        // - Interpolation -
        
        bool setInterpAlgo( interpTypes interpAlgo ) { this->interpolAlgo = interpAlgo; return true; }
        
        // - Misc -
        
        inline unsigned int nOfFrames( void ) { return data.n_cols; }
        inline unsigned int nOfNodes( void ) { return 1; }
        void clear( void ) { data.clear(); }
    
      protected:
        
        bool checkInput( arma::vec pData );
        
        interpTypes interpolAlgo;
        arma::mat data;
    };
    
    // - Inlined functions -
    
    bool timed2dContainer::pushFrame( arma::vec pData ) {
        
        if( timedFlag ) return false; // First check if conditions to push
        if( data.n_elem > 0 && pData.n_rows != data.n_rows ) return false;
        
        data.insert_cols( data.n_cols, pData );
        
        return true;
    }
    
    void timed2dContainer::pop_front( void ) {
        
        data.shed_col( 0 ); // Shed one col/vec
        if( timedFlag ) timeStamp.shed_row( 0 );
    }
    
    inline timed2dContainer timed2dContainer::chop( int pBegIndex, int pEndIndex ) {
        
        timed2dContainer oneMat = *this;
        
        oneMat.data = data.submat( 0, pBegIndex, data.n_rows-1, pEndIndex ); // Chopping the data matrix and timestamps (if any)
        if( isTimestamped() ) oneMat.getRefTimeStamps() = getRefTimeStamps().subvec( pBegIndex, pEndIndex );
        
        return( oneMat );
    }
    
    // - 3D timed container -
    
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

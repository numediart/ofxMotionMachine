//
//  mmContainer.cpp
//  MoMaBrouillon
//
//  Created by Thierry Ravet on 18/06/15.
//
//

#include "mmContainer.h"

namespace MoMa {
    void timedContainer::interpIndexFind(const arma::vec pVec, double pValue, unsigned int &index1, double &weight1, unsigned int &index2, double &weight2 ){
        unsigned int lIndex1;
        arma::abs(pVec-pValue).min(lIndex1);
        if (pVec(lIndex1)==pValue){
            index1=lIndex1;
            index2=lIndex1;
            weight1=1.0;
            weight2=0.0;
            return;
        }
        else{
            if (pVec(lIndex1)<pValue){
                if (lIndex1==0){
                    index1=lIndex1;
                    index2=lIndex1;
                    weight1=1.0;
                    weight2=0.0;
                    return;
                }
                index1=lIndex1;
                index2=lIndex1+1;
            }
            else{
                if (lIndex1==pVec.size()-1){
                    index1=lIndex1;
                    index2=lIndex1;
                    weight1=1.0;
                    weight2=0.0;
                    return;
                }
                index2=lIndex1;
                index1=lIndex1-1;
            }
            weight1=(pVec(index2)-pValue)/(pVec(index2)-pVec(index1));
            weight2=1-weight1;
            return;
        }
    }
    
    bool timedContainer::timeStampIsValid(arma::vec pTime){
        if (pTime.n_elem==0)
            return false;
        arma::vec::iterator it1,it2;
        it1=pTime.begin();
        it2=it1+1;
        while (it2!=pTime.end()){//verify if pTIme is monotonic increasing vector
            if (*it1>*it2)
                return false;
            it1++;
            it2++;
        }
        return true;
    }
//    timed1dContainer
    
    bool timed1dContainer::pushTimedFrame(double pData,double pTime){
        if (pTime<this->timeStamp(timeStamp.n_elem-1))
            return false;
        this->setTimedFrame(pData,pTime);
        return true;
    }
    double timed1dContainer::getTimedFrame(double pTime){
        if (timedFlag){
            double weight1,weight2;
            unsigned int index1,index2;
            interpIndexFind(this->timeStamp,pTime,index1,weight1,index2,weight2);
            return ((weight1*data(index1)+weight2*data(index2))/(weight1+weight2));
        }
        else {
            return this->data((int)(pTime*frameRate));
        }
        
        return 0.0;
    }
    bool timed1dContainer::setTimedFrame(double pData,double pTime){
        if (timedFlag){
            if(timeStamp.n_elem==0)
                return false;
            if (timeStamp(0)>pTime){
                data.insert_rows(0, 1);
                data(0)=pData;
                timeStamp.insert_rows(0, 1);
                timeStamp(0)=pTime;
                return true;
            }
            if (timeStamp(timeStamp.n_elem-1)<pTime){
                data.insert_rows(data.n_elem, 1);
                data(data.n_elem-1)=pData;
                timeStamp.insert_rows(timeStamp.n_elem, 1);
                timeStamp(timeStamp.n_elem-1)=pTime;
                return true;
            }
            arma::uvec index=find(this->timeStamp==pTime);
            if (index.n_elem>0){
                data(index(0))=pData;
                return true;
            }
            else{
                index=find(this->timeStamp>pTime);
                if (index.n_elem>0){
                    data.insert_rows(index(0), 1);
                    data(index(0))=pData;
                    timeStamp.insert_rows(index(0), 1);
                    timeStamp(index(0))=pTime;
                    return true;
                }
                else
                    return false;
            }
        }
        else {
            if ( std::abs((pTime-frameRate*round(pTime/frameRate))) <= 1E-6 ) {
                
                if ((int)round(pTime/frameRate)<data.n_elem){
                    data((int)round(pTime/frameRate))=pData;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
    bool timed1dContainer::setIndexedFrame(double pData,unsigned int pIndex){
        if (timedFlag)
            return false;
        if (pIndex<data.n_elem){
            data(pIndex)=pData;
            return true;
        }
        else{

            if (pIndex==data.n_elem){
                data.insert_rows(data.n_elem, 1);
                data(data.n_elem-1)=pData;
                return true;
			}
            else{
				return false;
			}
			//int jun;
		}
        return true;
    }
    bool timed1dContainer::setData(float pFrameRate, arma::vec pData){
        if (pFrameRate<=0.0)
            return false;
        timedFlag=false;
        frameRate=pFrameRate;
        data=pData;
        return true;
        
    }
    bool timed1dContainer::setTimedData(arma::vec pTime, arma::vec pData){
        if (timeStampIsValid(pTime)==false)
            return false;
        if (pTime.n_elem!=pData.n_elem)
            return false;
        timedFlag=true;
        timeStamp=pTime;
        data=pData;
        return true;
    }
    

//timed2dContainer
    
    bool timed2dContainer::pushTimedFrame(arma::vec  pData,double pTime){
        if (pTime<this->timeStamp(timeStamp.n_elem-1))
            return false;
        this->setTimedFrame(pData,pTime);
        return true;
        
    }
    bool timed2dContainer::checkInput(arma::vec pData){
        if (data.n_elem>0&&pData.n_rows!=data.n_rows)
            return false;
        else
            return true;
    }
    arma::vec timed2dContainer::getTimedFrame(double pTime){
        if (timedFlag){
            double weight1,weight2;
            unsigned int index1,index2;
            interpIndexFind(this->timeStamp,pTime,index1,weight1,index2,weight2);
            return ((weight1*data.col(index1)+weight2*data.col(index2))/(weight1+weight2));
            //TODO : to implement qslerp
        }
        else {
            if ((pTime*frameRate)>(data.n_elem-1)){
                return arma::zeros(1,1);
            }
            else
                return this->data.col((int)(pTime*frameRate));
        }
        
        return arma::zeros(1,1);
    }
    
    bool timed2dContainer::setTimedFrame(arma::vec pData,double pTime){
        if (!checkInput(pData))
            return false;
        if (timedFlag){
            if(timeStamp.n_elem==0)
                return false;
            if (timeStamp(0)>pTime){
                data.insert_cols(0, 1);
                data.col(0)=pData;
                timeStamp.insert_rows(0, 1);
                timeStamp(0)=pTime;
                return true;
            }
            if (timeStamp(timeStamp.n_elem-1)<pTime){
                data.insert_cols(data.n_cols, 1);
                data.col(data.n_cols-1)=pData;
                timeStamp.insert_rows(timeStamp.n_elem, 1);
                timeStamp(timeStamp.n_elem-1)=pTime;
                return true;
            }
            arma::uvec index=find(this->timeStamp==pTime);
            if (index.n_elem>0){
                data.col(index(0))=pData;
                return true;
            }
            else{
                index=find(this->timeStamp>pTime);
                if (index.n_elem>0){
                    data.insert_cols(index(0), 1);
                    data.col(index(0))=pData;
                    timeStamp.insert_rows(index(0), 1);
                    timeStamp(index(0))=pTime;
                    return true;
                }
                else
                    return false;
            }
        }
        else {
            if ( std::abs(pTime-frameRate*round(pTime/frameRate)) <= 1E-6 ) {
                
                if ((int)round(pTime/frameRate)<data.n_cols){
                    data.col((int)round(pTime/frameRate))=pData;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
    bool timed2dContainer::setIndexedFrame(arma::vec pData,unsigned int pIndex){
        if (!checkInput(pData))
            return false;
        if (timedFlag)
            return false;
        if (pIndex<data.n_elem){
            data.col(pIndex)=pData;
            return true;
        }
        else{
            if (pIndex==data.n_elem){
                data.insert_cols(data.n_cols, 1);
                data.col(data.n_cols-1)=pData;
                return true;
            }
            else
                return false;
        }
        return true;
    }
    bool timed2dContainer::setData(const float pFrameRate, const arma::mat pData){
        if (pFrameRate<=0.0)
            return false;
        timedFlag=false;
        frameRate=pFrameRate;
        this->data=pData;
        return true;
        
    }
    bool timed2dContainer::setTimedData(const arma::vec pTime, const arma::mat pData){
        if (timeStampIsValid(pTime)==false)
            return false;
        if (pTime.n_elem!=pData.n_cols)
            return false;
        timedFlag=true;
        timeStamp=pTime;
        data=pData;
        return true;
    }
    

    
//timed3dContainer
    
    bool timed3dContainer::pushTimedFrame(arma::mat  pData,double pTime){
        if (pTime<this->timeStamp(timeStamp.n_elem-1))
            return false;
        this->setTimedFrame(pData,pTime);
        return true;
    }
    bool timed3dContainer::checkInput(arma::mat pData){
        if (data.n_elem>0&&(pData.n_rows!=data.n_rows||pData.n_cols!=data.n_cols))
            return false;
        else
            return true;
    }
    

    arma::mat timed3dContainer::getTimedFrame(double pTime){
        if (timedFlag){
            double weight1,weight2;
            unsigned int index1,index2;
            interpIndexFind(this->timeStamp,pTime,index1,weight1,index2,weight2);
            if (interpolAlgo==linear)
                return ((weight1*data.slice(index1)+weight2*data.slice(index2))/(weight1+weight2));
            else if (interpolAlgo==qslerp) {
                
                // TODO : to implement qslerp
            }
            else
                return arma::zeros(1,1);
            
                
        }
        else {
            return this->data.slice((int)(pTime*frameRate));
        }
        
        return arma::zeros(1,1);
    }

	bool timed3dContainer::setTimedFrame(const arma::mat pData,const double pTime){
        if (!checkInput(pData))
            return false;
        if (timedFlag){
            if(timeStamp.n_elem==0)
                return false;
            if (timeStamp(0)>pTime){
                data.insert_slices(0, 1);
                data.slice(0)=pData;
                timeStamp.insert_rows(0, 1);
                timeStamp(0)=pTime;
                return true;
            }
            if (timeStamp(timeStamp.n_elem-1)<pTime){
                data.insert_slices(data.n_slices, 1);
                data.slice(data.n_slices-1)=pData;
                timeStamp.insert_rows(timeStamp.n_elem, 1);
                timeStamp(timeStamp.n_elem-1)=pTime;
                return true;
            }
            arma::uvec index=find(this->timeStamp==pTime);
            if (index.n_elem>0){
                data.slice(index(0))=pData;
                return true;
            }
            else{
                index=find(this->timeStamp>pTime);
                if (index.n_elem>0){
                    data.insert_slices(index(0), 1);
                    data.slice(index(0))=pData;
                    timeStamp.insert_rows(index(0), 1);
                    timeStamp(index(0))=pTime;
                    return true;
                }
                else
                    return false;
            }
        }
        else {
			if ( std::abs(pTime-this->frameRate*round(pTime/frameRate)) <= 1E-6 ) {
                if ((int)round(pTime/frameRate)<data.n_slices){
                    data.slice((int)round(pTime/frameRate))=pData;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
    bool timed3dContainer::setIndexedFrame(const arma::mat pData,unsigned int pIndex){
        if (!checkInput(pData))
            return false;
        if (timedFlag)
            return false;
        if (pIndex<data.n_elem){
            data.slice(pIndex)=pData;
            return true;
        }
        else{
            if (pIndex==data.n_elem){
                data.insert_slices(data.n_slices, 1);
                data.slice(data.n_slices-1)=pData;
                return true;
            }
            else
                return false;
        }
        return true;
    }
    bool timed3dContainer::setData(float pFrameRate,const arma::cube pData){
        if (pFrameRate<=0.0)
            return false;
        timedFlag=false;
        frameRate=pFrameRate;
        data=pData;
        return true;
        
    }
    bool timed3dContainer::setTimedData(const arma::vec pTime,const arma::cube pData){
        if (timeStampIsValid(pTime)==false)
            return false;
        if (pTime.n_elem!=pData.n_slices)
            return false;
        timedFlag=true;
        timeStamp=pTime;
        data=pData;
        return true;
    }
};

//
//  mmContainer.cpp
//  MoMaBrouillon
//
//  Created by Thierry Ravet on 18/06/15.
//
//

#include "mmContainer.h"
#include <cmath>

namespace MoMa {
        
    
    bool TimedData::interpIndexFind(const arma::vec pVec, double pValue, unsigned int &index1, double &weight1, unsigned int &index2, double &weight2 ) const{
        unsigned int lIndex1;
		arma::uword ltemp;
		arma::abs(pVec-pValue).min(ltemp);
		lIndex1=(unsigned int)ltemp;
        if ( fabs(pVec(lIndex1)-pValue ) <= 1E-6 ){
            index1=lIndex1;
            index2=lIndex1;
            weight1=1.0;
            weight2=0.0;
            return true;
        }
		if (pValue<pVec((mLastId+1)%pVec.size())){
            
			index1=lIndex1;
            index2=lIndex1;
            weight1=0;
            weight2=1;
            if( ( pVec( ( mLastId + 1 ) % pVec.size() ) - pValue ) > 1E-6 )
                return false;
            return true;
		}
		if (pValue>pVec((mLastId)%pVec.size())){
			index1=lIndex1;
            index2=lIndex1;
            weight1=1;
            weight2=0;
            if( ( pValue - pVec( mLastId % pVec.size() ) ) > 1E-6 )
                return false;
			return true;
		}
		if (pVec(lIndex1)<pValue){
	        index1=lIndex1;
			index2=lIndex1+1;
		}
		else{
			index2=lIndex1;
			index1=lIndex1-1;
		}
		weight1=(pVec(index2)-pValue)/(pVec(index2)-pVec(index1));
		weight2=1-weight1;
        if( ( pVec( index2 ) - pVec( index1 ) ) > 2 * mInterpValidTime )
            return false;
		return true;
	}
    
	
    bool TimedData::checkTimeVec( arma::vec pTime,unsigned int pLastId ) {
        
        if (pTime.n_elem==0)
            return false;
        arma::vec::iterator it1,it2;
        it1=pTime.begin();
        it2=it1+1;
        while (it2!=pTime.end()){//verify if pTIme is monotonic increasing vector
			if ((*it1>*it2)&&it1!=pTime.begin()+pLastId)
                return false;
            it1++;
            it2++;
        }
        return true;
    }
    
    
    bool TimedData::checkTimeVec( arma::vec pTime ) {
        
        if (pTime.n_elem==0)
            return false;
        arma::vec::iterator it1,it2;
        it1=pTime.begin();
        it2=it1+1;
        while (it2!=pTime.end()){//verify if pTIme is monotonic increasing vector
			if ((*it1>*it2)&&it1!=pTime.begin()+mLastId)
                return false;
            it1++;
            it2++;
        }
        return true;
    }
	unsigned int TimedData::checkLastId(const arma::vec &pTime){
		unsigned int lLastId=0;
        arma::vec::const_iterator it1;
        it1=pTime.begin();
		while ((it1+1)!=pTime.end()&&(*(it1+1)>*it1)){
			it1++;
			lLastId++;
		}
		return lLastId;
	}
// == TIMED VEC ==
	bool TimedVec::SetValidParam(){
		if (mTimed){
			if (mTimeVec.n_elem>0)
				if (mTimeVec.n_elem!=mData.n_elem)
					return false;}
		else
			if (mFrameRate<=0.0)
				return false;
		if (!mIsRealTime){
			if (mData.n_elem>0){
				mBufferSize=mData.n_elem;
				mIsFilled=true;
				mLastId=mBufferSize-1;
			}
		}
		else{
			if (mData.n_elem>0){
				mBufferSize=mData.n_elem;
				mIsFilled=true;
				if (mTimed)
					mLastId=checkLastId(this->mTimeVec);
				else
					mLastId=mBufferSize-1;
			}
		}
		return true;
	};

    
    bool TimedVec::setData( double pFrameRate, const arma::vec &pData, float initTime) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
        mData = pData;
		mLastId=mData.n_elem-1;
		mBufferSize=mData.n_elem;
		mIsFilled=true;

        mInitialTime = initTime;
        
        return true;
    }
    
    bool TimedVec::setData( const arma::vec &pTime, const arma::vec &pData ) {
		
        if( pTime.n_elem != pData.n_elem ) return false;
		if (mIsRealTime)
			mLastId=checkLastId(pTime);
		else		
			mLastId=pTime.n_elem-1;
		mIsFilled=true;
        if( checkTimeVec(pTime) == false ) return false;//TODO throw exception
        mTimeVec = pTime;
        mTimed = true;
        mData = pData;
		mBufferSize=mData.n_elem;
        return true;
    }
    bool TimedVec::swapData( double pFrameRate,  arma::vec &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        mFrameRate = pFrameRate;
        mTimed = false;
		mData.clear();
        mData.swap(pData);
		mLastId=mData.n_elem-1;
		mBufferSize=mData.n_elem;
		mIsFilled=true;
        return true;
    }
    
    bool TimedVec::swapData(  arma::vec &pTime,  arma::vec &pData ) {
        if( pTime.n_elem != pData.n_elem ) return false;
        
		if (mIsRealTime)
			mLastId=checkLastId(pTime);
		else		
			mLastId=pTime.n_elem-1;
        if( checkTimeVec(pTime) == false ) return false;//TODO throw exception
		mTimeVec.clear();
        mTimeVec.swap(pTime);
        mTimed = true;
		mData.clear();
        mData.swap(pData);
		mBufferSize=mData.n_elem;
		mIsFilled=true;
        return true;
    }

    TimedVec TimedVec::sub( int pBegIndex, int pEndIndex ) const{
    
        TimedVec oneVec;
		pBegIndex=memIndex(pBegIndex);
		pEndIndex=memIndex(pEndIndex);
        if( isTimed() )
			if (pEndIndex>=pBegIndex)
				oneVec.setData(this->getTimeVec().subvec( pBegIndex, pEndIndex ),this->mData.subvec( pBegIndex, pEndIndex ));
			else{
				arma::vec lData(pEndIndex-pBegIndex+1+mBufferSize);
				lData.subvec(0,mBufferSize-1-pBegIndex)=this->mData.subvec( pBegIndex,mBufferSize-1);
				lData.subvec(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mData.subvec( 0,pEndIndex);
				arma::vec lTime(pEndIndex-pBegIndex+1+mBufferSize);
				lTime.subvec(0,mBufferSize-1-pBegIndex)=this->mTimeVec.subvec( pBegIndex,mBufferSize-1);
				lTime.subvec(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mTimeVec.subvec( 0,pEndIndex);
				oneVec.swapData(lTime,lData);
			}

        else
        {
			if (pEndIndex>=pBegIndex)
            oneVec.setData(mFrameRate,this->mData.subvec( pBegIndex, pEndIndex ));
			else{
				arma::vec lData(mBufferSize+pEndIndex-pBegIndex+1);
				lData.subvec(0,mBufferSize-1-pBegIndex)=this->mData.subvec( pBegIndex,mBufferSize-1);
				lData.subvec(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mData.subvec( 0,pEndIndex);
				oneVec.swapData(mFrameRate,lData);
			}
			oneVec.setInitialTime(this->mInitialTime+(double)usedIndex(pBegIndex)/mFrameRate);
        }
        
        return( oneVec );
    }
	TimedVec TimedVec::getOfflineData( ) const{
		TimedVec oneVec;
		unsigned int pBegIndex=memIndex(0);
		unsigned int pEndIndex=mLastId%mBufferSize;
        if( isTimed() )
			if (pEndIndex>=pBegIndex)
				oneVec.setData(this->getTimeVec().subvec( pBegIndex, pEndIndex ),this->mData.subvec( pBegIndex, pEndIndex ));
			else{
				arma::vec lData(pEndIndex-pBegIndex+1+mBufferSize);
				lData.subvec(0,mBufferSize-1-pBegIndex)=this->mData.subvec( pBegIndex,mBufferSize-1);
				lData.subvec(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mData.subvec( 0,pEndIndex);
				arma::vec lTime(pEndIndex-pBegIndex+1+mBufferSize);
				lTime.subvec(0,mBufferSize-1-pBegIndex)=this->mTimeVec.subvec( pBegIndex,mBufferSize-1);
				lTime.subvec(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mTimeVec.subvec( 0,pEndIndex);
				oneVec.swapData(lTime,lData);
			}

        else
        {
			if (pEndIndex>=pBegIndex)
				oneVec.setData(mFrameRate,this->mData.subvec( pBegIndex, pEndIndex ));
			else{
				arma::vec lData(mBufferSize+pEndIndex-pBegIndex+1);
				lData.subvec(0,mBufferSize-1-pBegIndex)=this->mData.subvec( pBegIndex,mBufferSize-1);
				lData.subvec(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mData.subvec( 0,pEndIndex);
				oneVec.swapData(mFrameRate,lData);
			}
			oneVec.setInitialTime(this->mInitialTime);
        }
        
        return( oneVec );
	}
	bool TimedVec::setRealTimeMode(unsigned int bufferSize,double initData){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=true;
		mData=initData*arma::ones(bufferSize,1);
		mTimeVec.zeros(bufferSize,1);
		return true;
	}
	
	bool TimedVec::setRealTimeMode(unsigned int bufferSize,double pFrameRate,double initData){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=false;
		mData=initData*arma::ones(bufferSize,1);
		mFrameRate=pFrameRate;
		mInitialTime=0.0;
		return true;
	}
	
    bool TimedVec::set( double pData, unsigned int pIndex ) {
        if( mTimed ) return false;
        if (mIsRealTime)
			return false;
        
		pIndex=memIndex(pIndex);
        
        if( pIndex < mData.n_elem ) {
            
            mData(pIndex) = pData;
            return true;
        
        } else {
            
            if( pIndex == mData.n_elem ) {
                
                mData.insert_rows( mData.n_elem, 1 );
                mData( mData.n_elem-1 ) = pData;
                return true;
            }
            
            else return false;
        }
        
        return true;
    }
    
    bool TimedVec::set( double pData, double pTime ) {
		if (mIsRealTime)
			return false;
        if( mTimed ) {
            
            if( mTimeVec.n_elem == 0 ) return false;
            
          /*  if( mTimeVec(0) > pTime ) {
                
                mData.insert_rows( 0, 1 );
                mData(0) = pData;
                mTimeVec.insert_rows( 0, 1 );
                mTimeVec(0) = pTime;
                
                return true;
            }
            
            if( mTimeVec( mTimeVec.n_elem-1 ) < pTime ) {
                
                mData.insert_rows( mData.n_elem, 1 );
                mData( mData.n_elem-1 ) = pData;
                mTimeVec.insert_rows( mTimeVec.n_elem, 1 );
                mTimeVec( mTimeVec.n_elem-1 ) = pTime;
                
                return true;
            }*/
			unsigned int index;
			arma::uword ltemp;
			arma::abs(this->mTimeVec - pTime).min(ltemp);
			index=(unsigned int)ltemp;
			if ( fabs(this->mTimeVec(index)-pTime ) <= 1E-6 ){
				mData(index)=pData;
				return true;
			}
			if (this->mTimeVec(index)<pTime)
				index++;
	        mData.insert_rows( index, 1 );
            mData( index  )= pData;
            mTimeVec.insert_rows( index, 1 );
            mTimeVec( index ) = pTime;
            return true;
		} 
		else {
            double lTime=pTime-mInitialTime;
            if (lTime<0.0)
                return false;
            if( fabs( (lTime-mFrameRate*round(lTime/mFrameRate)) ) <= 1E-6 ) {
                
                if( (int)round(lTime/mFrameRate) < mData.n_elem ) {
                    
                    mData( ((int)round(lTime/mFrameRate))) = pData;
                    
                    return true;
                
                } else return false;
            
            } else return false;
        }
    }
    
    bool TimedVec::push( double pData, double pTime ) {
		if (!mTimed||!mIsRealTime) return false;
		if( pTime <= this->maxTime()) return false;
		mLastId++;
		if (mLastId>=mBufferSize)
			mLastId=0;
		mData(mLastId)=pData;
		mTimeVec(mLastId)=pTime;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    }
    
    bool TimedVec::push( double pData ) {
        
		if (mTimed||!mIsRealTime) return false;
		mLastId++;
		if (mLastId>=mBufferSize)
			mLastId=0;
		mData(mLastId)=pData;
		if (mIsFilled)
			mInitialTime+=1.0/mFrameRate;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    };
	
		
	bool TimedVec::push(const arma::vec &pData,const arma::vec &pTime){
		if (!mIsRealTime||!mTimed)
			return false;
		if (pData.n_elem!=pTime.n_elem)
			return false;
		if ((!checkTimeVec(pTime,pTime.n_elem))||(pTime(0)<=(mLastId==mBufferSize?0.0:mTimeVec(mLastId))))
			return false;
		if (pData.n_elem>=mBufferSize){
			mData=pData.subvec(pData.n_elem-mBufferSize, pData.n_elem-1);
			mTimeVec=pTime.subvec(pTime.n_elem-mBufferSize, pTime.n_elem-1);
			mLastId=mBufferSize-1;
			
					mIsFilled=true;
		}
		else{
			unsigned int begInd=(mLastId+1)*((mLastId+1)<mBufferSize);
			unsigned int endInd=((mLastId+1)*((mLastId+1)<mBufferSize)+(mData.n_elem-1))%mBufferSize;
			if (begInd<=endInd){
				mData.subvec(begInd,endInd)=pData;
				mTimeVec.subvec(begInd,endInd)=pTime;
								
				if (endInd==(mBufferSize-1))
					mIsFilled=true;
			}
			else{
				mData.subvec(begInd,mBufferSize-1)=pData.subvec(0,mBufferSize-begInd-1);
				mData.subvec(0,endInd)=pData.subvec(mBufferSize-begInd,pData.n_elem-1);
				mTimeVec.subvec(begInd,mBufferSize-1)=pTime.subvec(0,mBufferSize-begInd-1);
				mTimeVec.subvec(0,endInd)=pTime.subvec(mBufferSize-begInd,pTime.n_elem-1);
				
					mIsFilled=true;
			}
			mLastId=endInd;
		}

        return true;
	};
	
	bool TimedVec::push(const arma::vec &pData){
		if (!mIsRealTime||mTimed)
			return false;
		if (mIsFilled)
			mInitialTime+=(double)pData.n_cols/mFrameRate;
		else
			if (mLastId*(mLastId!=mBufferSize)+pData.n_cols>=mBufferSize)
				mInitialTime+=(double)(pData.n_cols+(1+mLastId)*(mLastId!=mBufferSize)-mBufferSize)/mFrameRate;
		if (pData.n_elem>=mBufferSize){
			mData=pData.subvec(pData.n_elem-mBufferSize, pData.n_elem-1);
			mLastId=mBufferSize-1;
			
					mIsFilled=true;
		}
		else{
			unsigned int begInd=(mLastId+1)*((mLastId+1)<mBufferSize);
			unsigned int endInd=((mLastId+1)*((mLastId+1)<mBufferSize)+(mData.n_elem-1))%mBufferSize;
			if (begInd<=endInd){
				mData.subvec(begInd,endInd)=pData;
				
				
				if (endInd==(mBufferSize-1))
					mIsFilled=true;
			}
			else{
				mData.subvec(begInd,mBufferSize-1)=pData.subvec(0,mBufferSize-begInd-1);
				mData.subvec(0,endInd)=pData.subvec(mBufferSize-begInd,pData.n_elem-1);
				
					mIsFilled=true;
			}
			mLastId=endInd;
		}

        return true;
	};
// == TIMED MAT ==
    bool TimedMat::SetValidParam(){
		if (mTimed){
			if (mTimeVec.n_elem>0)
				if (mTimeVec.n_elem!=mData.n_cols)
					return false;}
		else
			if (mFrameRate<=0.0)
				return false;
		if (!mIsRealTime){
			if (mData.n_cols>0){
				mBufferSize=mData.n_cols;
				mIsFilled=true;
				mLastId=mBufferSize-1;
			}
		}
		else{
			if (mData.n_cols>0){
				mBufferSize=mData.n_cols;
				mIsFilled=true;
				if (mTimed)
					mLastId=checkLastId(this->mTimeVec);
				else
					mLastId=mBufferSize-1;
			}
		}
		return true;
	};
    bool TimedMat::setInterpAlgo( InterpTypes interpAlgo ) {
        
        this->interpolAlgo = interpAlgo;
        
        return true;
    }
    
    bool TimedMat::checkInput( const arma::vec &pData ) {
        
        if( mData.n_elem > 0 && pData.n_rows != mData.n_rows ) return false;//TODO throw exception
        else return true;
    }
    
    bool TimedMat::setData(  double pFrameRate, const arma::mat &pData, float initTime) {
        
        if( pFrameRate<=0.0 ) return false;
        
        mFrameRate = pFrameRate;
        this->mData = pData;
        mTimed = false;
		mLastId=mData.n_cols-1;
		mBufferSize=mData.n_cols;
		mIsFilled=true;
        
        mInitialTime = initTime;

        return true;
    }
    
    bool TimedMat::setData( const arma::vec &pTime, const arma::mat &pData ) {
        if( pTime.n_elem != pData.n_cols ) return false;
		if (mIsRealTime)
			mLastId=checkLastId(pTime);
		else		
			mLastId=pTime.n_elem-1;
        if( checkTimeVec(pTime) == false ) return false;
        
        
        mTimeVec = pTime;
        mTimed = true;
        mData = pData;

		mBufferSize=mData.n_cols;
		mIsFilled=true;
        return true;
    }
 bool TimedMat::swapData( double pFrameRate,  arma::mat &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
		mData.clear();
        mData.swap(pData);
		mLastId=mData.n_cols-1;
		mBufferSize=mData.n_cols;
		mIsFilled=true;
        return true;
    }
    
    bool TimedMat::swapData(  arma::vec &pTime,  arma::mat &pData ) {
		if( pTime.n_elem != pData.n_cols ) return false;
		if (mIsRealTime)
			mLastId=checkLastId(pTime);
		else		
			mLastId=pTime.n_elem-1;
        if( checkTimeVec(pTime) == false ) return false;//TODO throw exception
		mTimeVec.clear();
        mTimeVec.swap(pTime);
        mTimed = true;
		mData.clear();
        mData.swap(pData);
		mBufferSize=mData.n_cols;
		mIsFilled=true;
        return true;
    } 
	
	bool TimedMat::setRealTimeMode(unsigned int bufferSize,arma::vec initData){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=true;
		mData=initData*arma::ones(1,bufferSize);
		mTimeVec.zeros(bufferSize,1);
		return true;
	}
	
	bool TimedMat::setRealTimeMode(unsigned int bufferSize,double pFrameRate,arma::vec initData){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=false;
		mData=initData*arma::ones(1,bufferSize);
		mFrameRate=pFrameRate;
		mInitialTime=0.0;
		return true;
	}
	bool TimedMat::setRealTimeMode(unsigned int bufferSize,unsigned int pDim){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=true;
		mData.clear();
		mData.zeros(pDim,bufferSize);
		mTimeVec.zeros(bufferSize,1);
		return true;
	}
	
	bool TimedMat::setRealTimeMode(unsigned int bufferSize,double pFrameRate,unsigned int pDim){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=false;
		mData.clear();
		mData.resize(pDim,bufferSize);
		mFrameRate=pFrameRate;
		mInitialTime=0.0;
		return true;
	}
	
    TimedMat TimedMat::sub( int pBegIndex, int pEndIndex ) const {
        
        TimedMat oneMat;
		 pBegIndex=memIndex(pBegIndex);
		 pEndIndex=memIndex(pEndIndex);
        if( isTimed() )
			if (pEndIndex>=pBegIndex)
				oneMat.setData(this->getTimeVec().subvec( pBegIndex, pEndIndex ),this->mData.cols( pBegIndex, pEndIndex ));
			else{
				arma::mat lData(mData.n_rows,mBufferSize+pEndIndex-pBegIndex+1);
				lData.cols(0,mBufferSize-1-pBegIndex)=this->mData.cols( pBegIndex,mBufferSize-1);
				lData.cols(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mData.cols( 0,pEndIndex);
				arma::vec lTime(mBufferSize+pEndIndex-pBegIndex+1);
				lTime.subvec(0,mBufferSize-1-pBegIndex)=this->mTimeVec.subvec( pBegIndex,mBufferSize-1);
				lTime.subvec(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mTimeVec.subvec( 0,pEndIndex);
				oneMat.swapData(lTime,lData);
			}
        else
        {
			if (pEndIndex>=pBegIndex)
            oneMat.setData(mFrameRate,this->mData.cols( pBegIndex, pEndIndex ));
			else{
				arma::mat lData(mData.n_rows,pEndIndex-pBegIndex+1+mBufferSize);
				lData.cols(0,mBufferSize-1-pBegIndex)=this->mData.cols( pBegIndex,mBufferSize-1);
				lData.cols(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mData.cols( 0,pEndIndex);
				oneMat.swapData(mFrameRate,lData);
			}
			oneMat.setInitialTime(this->mInitialTime+(double)usedIndex(pBegIndex)/mFrameRate);
        }
		return oneMat;
        
    }
    
	TimedMat TimedMat::getOfflineData( ) const{
		unsigned int pBegIndex=memIndex(0);
		unsigned int pEndIndex=mLastId%mBufferSize;
        TimedMat oneMat;
       if( isTimed() )
			if (pEndIndex>=pBegIndex)
				oneMat.setData(this->getTimeVec().subvec( pBegIndex, pEndIndex ),this->mData.cols( pBegIndex, pEndIndex ));
			else{
				arma::mat lData(mData.n_rows,mBufferSize+pEndIndex-pBegIndex+1);
				lData.cols(0,mBufferSize-1-pBegIndex)=this->mData.cols( pBegIndex,mBufferSize-1);
				lData.cols(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mData.cols( 0,pEndIndex);
				arma::vec lTime(mBufferSize+pEndIndex-pBegIndex+1);
				lTime.subvec(0,mBufferSize-1-pBegIndex)=this->mTimeVec.subvec( pBegIndex,mBufferSize-1);
				lTime.subvec(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mTimeVec.subvec( 0,pEndIndex);
				oneMat.swapData(lTime,lData);
			}
        else
        {
			if (pEndIndex>=pBegIndex)
            oneMat.setData(mFrameRate,this->mData.cols( pBegIndex, pEndIndex ));
			else{
				arma::mat lData(mData.n_rows,pEndIndex-pBegIndex+1+mBufferSize);
				lData.cols(0,mBufferSize-1-pBegIndex)=this->mData.cols( pBegIndex,mBufferSize-1);
				lData.cols(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mData.cols( 0,pEndIndex);
				oneMat.swapData(mFrameRate,lData);
			}
			oneMat.setInitialTime(this->mInitialTime);
        }
		return oneMat;
        
	}
    TimedVec TimedMat::elem( unsigned int pIndex ) const {
    
        TimedVec elem;
        arma::vec elemVec;

        
        elemVec = mData.row( pIndex ).t();
		if( mTimed ){
			
			if (mIsRealTime) 
				elem.setRealTimeMode(mBufferSize,0);
			elem.setData( mTimeVec, elemVec );
		}
        else {
			if (mIsRealTime) 
				elem.setRealTimeMode(mBufferSize,mFrameRate,0);
            elem.setData( mFrameRate, elemVec );
            elem.setInitialTime(this->mInitialTime);
        }
		
		if (mIsRealTime)
			elem.setLastId(mLastId);
		elem.setIsFilled(mIsFilled);
        return( elem );
    }
    bool TimedMat::set( const arma::vec &pData, unsigned int pIndex ) {
        
		if (mIsRealTime)
			return false;
        if( mTimed ) return false;
        if( !checkInput(pData) ) return false;
        
		pIndex=memIndex(pIndex);
        
        if( pIndex<mData.n_cols ) {
            
            mData.col( pIndex ) = pData;
            
            return true;
        
        } else {
            
            if( pIndex == mData.n_cols ) {
                
                mData.insert_cols( mData.n_cols, 1 );
                mData.col( mData.n_cols-1 ) = pData;
                
                return true;
            
            } else return false;
        }
        
        return true;
    }
    
    bool TimedMat::set( const arma::vec &pData, double pTime ) {
		if (mIsRealTime)
			return false;
        
        if( !checkInput(pData) ) return( false );
        
        if( mTimed ) {
            
            if( mTimeVec.n_elem == 0 ) return false;
            
          /*  if( mTimeVec(0) > pTime ) {
                
                mData.insert_cols( 0, 1 );
                mData.col(0) = pData;
                mTimeVec.insert_rows( 0, 1 );
                mTimeVec(0) = pTime;
                
                return true;
            }
            
            if( mTimeVec( mTimeVec.n_elem-1 ) < pTime ) {
                
                mData.insert_cols( mData.n_elem, 1 );
                mData.col( mData.n_elem-1 ) = pData;
                mTimeVec.insert_rows( mTimeVec.n_elem, 1 );
                mTimeVec( mTimeVec.n_elem-1 ) = pTime;
                
                return true;
            }*/
			
			unsigned int index;
			arma::uword ltemp;
			arma::abs(this->mTimeVec - pTime).min(ltemp);
			index=(unsigned int)ltemp;
			if ( fabs(this->mTimeVec(index)-pTime ) <= 1E-6 ){
				mData.col(index)=pData;
				return true;
			}
			if (this->mTimeVec(index)<pTime)
				index++;
	        mData.insert_cols( index, 1 );
            mData.col( index  )= pData;
            mTimeVec.insert_rows( index, 1 );
            mTimeVec( index ) = pTime;
            return true;
        
        } else {
            
            double lTime=pTime-mInitialTime;
            if (lTime<0.0)
                return false;
            if( fabs( lTime-mFrameRate*round(lTime/mFrameRate) ) <= 1E-6 ) {
                
                if( (int)round(lTime/mFrameRate) < mData.n_cols ) {
                    
                    mData.col( (int)round(lTime/mFrameRate) ) = pData;
                    
                    return true;
                
                } else return false;
            
            } else return false;
        }
    }
    
   
    bool TimedMat::push(const arma::vec &pData, double pTime ) {
		if (!mTimed||!mIsRealTime) return false;
		if( pTime <= this->maxTime()) return false;

		mLastId++;
		if (mLastId>=mBufferSize)
			mLastId=0;
		mData.col(mLastId)=pData;
		mTimeVec(mLastId)=pTime;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    }
    
    bool TimedMat::push(const arma::vec & pData ) {
        
		if (mTimed||!mIsRealTime) return false;
		
		mLastId++;
		if (mLastId>=mBufferSize)
			mLastId=0;
		mData.col(mLastId)=pData;
		if (mIsFilled)
			mInitialTime+=1.0/mFrameRate;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    };
    bool TimedMat::push(const arma::mat &pData,const arma::vec &pTime){
		if (!mIsRealTime||!mTimed)
			return false;
		if (pData.n_cols!=pTime.n_elem)
			return false;
		if ((!checkTimeVec(pTime,pTime.n_elem))||(pTime(0)<=(mLastId==mBufferSize?0.0:mTimeVec(mLastId))))
			return false;
		if (pData.n_cols>=mBufferSize){
			mData=pData.cols(pData.n_cols-mBufferSize, pData.n_cols-1);
			mTimeVec=pTime.subvec(pTime.n_elem-mBufferSize, pTime.n_elem-1);
			mLastId=mBufferSize-1;
			mIsFilled=true;
		}
		else{
			unsigned int begInd=(mLastId+1)*((mLastId+1)<mBufferSize);
			unsigned int endInd=((mLastId+1)*((mLastId+1)<mBufferSize)+(pData.n_cols-1))%mBufferSize;
			if (begInd<=endInd){
				mData.cols(begInd,endInd)=pData;
				mTimeVec.subvec(begInd,endInd)=pTime;
				if (endInd==(mBufferSize-1))
					mIsFilled=true;
			}
			else{
				mData.cols(begInd,mBufferSize-1)=pData.cols(0,mBufferSize-begInd-1);
				mData.cols(0,endInd)=pData.cols(mBufferSize-begInd,pData.n_cols-1);
				mTimeVec.subvec(begInd,mBufferSize-1)=pTime.subvec(0,mBufferSize-begInd-1);
				mTimeVec.subvec(0,endInd)=pTime.subvec(mBufferSize-begInd,pTime.n_elem-1);
				mIsFilled=true;
			}
			mLastId=endInd;
		}
		return true;
	};
	
	bool TimedMat::push(const arma::mat &pData){
		if (!mIsRealTime||mTimed)
			return false;
		if (mIsFilled)
			mInitialTime+=(double)pData.n_cols/mFrameRate;
		else
			if (mLastId*(mLastId!=mBufferSize)+pData.n_cols>=mBufferSize)
				mInitialTime+=(double)(pData.n_cols+(1+mLastId)*(mLastId!=mBufferSize)-mBufferSize)/mFrameRate;
		if (pData.n_cols>=mBufferSize){
			mData=pData.cols(pData.n_cols-mBufferSize, pData.n_cols-1);
			mLastId=mBufferSize-1;
			mIsFilled=true;
		}
		else{
			unsigned int begInd=(mLastId+1)*((mLastId+1)<mBufferSize);
			unsigned int endInd=((mLastId+1)*((mLastId+1)<mBufferSize)+(pData.n_cols-1))%mBufferSize;
			if (begInd<=endInd){
				mData.cols(begInd,endInd)=pData;
				if (endInd==(mBufferSize-1))
					mIsFilled=true;
			}
			else{
				mData.cols(begInd,mBufferSize-1)=pData.cols(0,mBufferSize-begInd-1);
				mData.cols(0,endInd)=pData.cols(mBufferSize-begInd,pData.n_cols-1);
				mIsFilled=true;
			}
			mLastId=endInd;
		}
		return true;
	};
    
// == TIMED CUBE ==
    bool TimedCube::SetValidParam(){
		if (mTimed){
			if (mTimeVec.n_elem>0)
				if (mTimeVec.n_elem!=mData.n_slices)
					return false;}
		else
			if (mFrameRate<=0.0)
				return false;
		if (!mIsRealTime){
			if (mData.n_slices>0){
				mBufferSize=mData.n_slices;
				mIsFilled=true;
				mLastId=mBufferSize-1;
			}
		}
		else{
			if (mData.n_slices>0){
				mBufferSize=mData.n_slices;
				mIsFilled=true;
				if (mTimed)
					mLastId=checkLastId(this->mTimeVec);
				else
					mLastId=mBufferSize-1;
			}
		}
		return true;
	};
    bool TimedCube::setInterpAlgo( InterpTypes interpAlgo ) {
        
        this->interpolAlgo = interpAlgo;
        
        return( true );
    }
    
    bool TimedCube::checkInput( const arma::mat &pData ) {
        
        if( mData.n_elem > 0 && ( pData.n_rows != mData.n_rows
        || pData.n_cols != mData.n_cols ) ) return( false );
        else return( true );
    }
    
    bool TimedCube::setData( double pFrameRate, const arma::cube &pData, float initTime) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
        mData = pData;
		mLastId=mData.n_slices-1;
		mBufferSize=mData.n_slices;
		mIsFilled=true;
        mInitialTime = initTime;
        
        return( true );
    }
    
    bool TimedCube::setData( const arma::vec &pTime, const arma::cube &pData ) {
        if ( pTime.n_elem != pData.n_slices ) return false;
        if (mIsRealTime)
			mLastId=checkLastId(pTime);
		else		
			mLastId=pTime.n_elem-1;
        if ( checkTimeVec( pTime ) == false ) return false;
        
        mTimeVec = pTime;
        mTimed = true;
        mData = pData;
		mBufferSize=mData.n_slices;
		mIsFilled=true;
        
        return( true );
    }

	
 bool TimedCube::swapData( double pFrameRate,  arma::cube &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
		mData=arma::zeros(1,1,1);
        mData.swap(pData);
		mLastId=mData.n_slices-1;
		mBufferSize=mData.n_slices;
		mIsFilled=true;
        
        return true;
    }
    
    bool TimedCube::swapData(  arma::vec &pTime,  arma::cube &pData ) {
		if( pTime.n_elem != pData.n_slices ) 
			return false;
        if (mIsRealTime)
			mLastId=checkLastId(pTime);
		else		
			mLastId=pTime.n_elem-1;
        if( checkTimeVec(pTime) == false ) return false;//TODO throw exception
		mTimeVec.clear();
        mTimeVec.swap(pTime);
        mTimed = true;
		mData.clear();
		mData=arma::ones(1,1,1);
        mData.swap(pData);
		mBufferSize=mData.n_slices;
		mIsFilled=true;
        return true;
    }
	
	bool TimedCube::setRealTimeMode(unsigned int bufferSize,arma::mat initData){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=true;
		mData.clear();
		mData.resize(initData.n_rows,initData.n_cols,bufferSize);
		for (int i=0;i<bufferSize;i++)
			mData.slice(i)=initData;
		mTimeVec.zeros(bufferSize,1);
		return true;
	}
	
	bool TimedCube::setRealTimeMode(unsigned int bufferSize,double pFrameRate,arma::mat initData){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=false;
		mData.clear();
		mData.resize(initData.n_rows,initData.n_cols,bufferSize);
		for (int i=0;i<bufferSize;i++)
			mData.slice(i)=initData;
		mFrameRate=pFrameRate;
		mInitialTime=0.0;
		return true;
	}
	bool TimedCube::setRealTimeMode(unsigned int bufferSize,unsigned int pDim0,unsigned int pDim1){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=true;
		mData.clear();
		mData.resize(pDim0,pDim1,bufferSize);
		mTimeVec.zeros(bufferSize,1);
		return true;
	}
	
	bool TimedCube::setRealTimeMode(unsigned int bufferSize,double pFrameRate,unsigned int pDim0,unsigned int pDim1){
		this->initRealTime(bufferSize);
		mData.clear();
		mTimed=false;
		mData.clear();
		mData.resize(pDim0,pDim1,bufferSize);
		mFrameRate=pFrameRate;
		mInitialTime=0.0;
		return true;
	}

	TimedCube TimedCube::sub(double pBegTime, double pEndTime) const {
		return this->sub((int)this->nearestIndex(pBegTime), (int)this->nearestIndex(pEndTime));

	}
    TimedCube TimedCube::sub( int pBegIndex, int pEndIndex ) const {
        
        TimedCube oneCube;
		 pBegIndex=memIndex(pBegIndex);
		 pEndIndex=memIndex(pEndIndex);
        if( isTimed() )
			if (pEndIndex>=pBegIndex)
				oneCube.setData(this->getTimeVec().subvec( pBegIndex, pEndIndex ),this->mData.slices( pBegIndex, pEndIndex ));
			else{
				arma::cube lData(mData.n_rows,mData.n_cols,pEndIndex-pBegIndex+1+mBufferSize);
				lData.slices(0,mBufferSize-1-pBegIndex)=this->mData.slices( pBegIndex,mBufferSize-1);
				lData.slices(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mData.slices( 0,pEndIndex);
				arma::vec lTime(pEndIndex-pBegIndex+1+mBufferSize);
				lTime.subvec(0,mBufferSize-1-pBegIndex)=this->mTimeVec.subvec( pBegIndex,mBufferSize-1);
				lTime.subvec(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mTimeVec.subvec( 0,pEndIndex);
				oneCube.swapData(lTime,lData);
			}
        else
        {
			if (pEndIndex>=pBegIndex)
            oneCube.setData(mFrameRate,this->mData.slices( pBegIndex, pEndIndex ));
			else{
				arma::cube lData(mData.n_rows,mData.n_cols,(int)mBufferSize+pEndIndex-pBegIndex+1);
				lData.slices(0,mBufferSize-1-pBegIndex)=this->mData.slices( pBegIndex,mBufferSize-1);
				lData.slices(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mData.slices( 0,pEndIndex);
				oneCube.swapData(mFrameRate,lData);
			}
			oneCube.setInitialTime(this->mInitialTime+(double)usedIndex(pBegIndex)/mFrameRate);
        }
        
        return( oneCube );
    }
    
	TimedCube TimedCube::getOfflineData( ) const{
		
		unsigned int pBegIndex=memIndex(0);
		unsigned int pEndIndex=mLastId%mBufferSize;
		TimedCube oneCube;
        if( isTimed() )
			if (pEndIndex>=pBegIndex)
				oneCube.setData(this->getTimeVec().subvec( pBegIndex, pEndIndex ),this->mData.slices( pBegIndex, pEndIndex ));
			else{
				arma::cube lData(mData.n_rows,mData.n_cols,pEndIndex-pBegIndex+1+mBufferSize);
				lData.slices(0,mBufferSize-1-pBegIndex)=this->mData.slices( pBegIndex,mBufferSize-1);
				lData.slices(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mData.slices( 0,pEndIndex);
				arma::vec lTime(pEndIndex-pBegIndex+1+mBufferSize);
				lTime.subvec(0,mBufferSize-1-pBegIndex)=this->mTimeVec.subvec( pBegIndex,mBufferSize-1);
				lTime.subvec(mBufferSize-pBegIndex,pEndIndex-pBegIndex+mBufferSize)=this->mTimeVec.subvec( 0,pEndIndex);
				oneCube.swapData(lTime,lData);
			}
        else
        {
			if (pEndIndex>=pBegIndex)
            oneCube.setData(mFrameRate,this->mData.slices( pBegIndex, pEndIndex ));
			else{
				arma::cube lData(mData.n_rows,mData.n_cols,(int)mBufferSize+pEndIndex-pBegIndex+1);
				lData.slices(0,mBufferSize-1-pBegIndex)=this->mData.slices( pBegIndex,mBufferSize-1);
				lData.slices(mBufferSize-pBegIndex,mBufferSize+pEndIndex-pBegIndex)=this->mData.slices( 0,pEndIndex);
				oneCube.swapData(mFrameRate,lData);
			}
			oneCube.setInitialTime(this->mInitialTime);
        }
        
        return( oneCube );
    }
    TimedMat TimedCube::row( unsigned int pIndex ) {
        
        TimedMat trace;
        arma::mat traceMtx;
        
        traceMtx = mData.tube( pIndex, 0, pIndex, mData.n_cols-1 );
        
		if( mTimed ){ 
			if (mIsRealTime) 
				trace.setRealTimeMode(mBufferSize,traceMtx.n_rows);
			trace.setData( mTimeVec, traceMtx );
		}
        else {
			if (mIsRealTime) 
				trace.setRealTimeMode(mBufferSize,mFrameRate,traceMtx.n_rows);
            trace.setData( mFrameRate, traceMtx );
			trace.setLastId(mLastId);
            trace.setInitialTime(this->mInitialTime);
        }
		if (mIsRealTime)
			trace.setLastId(mLastId);
		trace.setIsFilled(mIsFilled);
        return( trace );
    }
    
    TimedMat TimedCube::col( unsigned int pIndex ) {
        
        TimedMat trace;
        arma::mat traceMtx;
        
        traceMtx = mData.tube( 0, pIndex, mData.n_rows-1, pIndex );
        
        if( mTimed ){
			if (mIsRealTime)  
				trace.setRealTimeMode(mBufferSize,traceMtx.n_rows);
			trace.setData( mTimeVec, traceMtx );
		}
        else {
			if (mIsRealTime)  
				trace.setRealTimeMode(mBufferSize,mFrameRate,traceMtx.n_rows);
            trace.setData( mFrameRate, traceMtx );
            trace.setInitialTime(this->mInitialTime);
        }
		
		if (mIsRealTime)
			trace.setLastId(mLastId);
		trace.setIsFilled(mIsFilled);
        
        return( trace );
    }
	  bool TimedCube::set( const arma::mat &pData, unsigned int pIndex ) {
		if (mIsRealTime)
			return false;
        if( mTimed ) return false;
        if( !checkInput( pData ) ) return false;
        // -> No go check: not timed or bad format
        
        pIndex=memIndex(pIndex);
        
        if( pIndex < mData.n_slices ) {
            
            // Insert mData into cube
            mData.slice( pIndex ) = pData;
            return true;
            
        } else {
            
            if( pIndex == mData.n_slices ) {
                
                // Insert mData at the end of cube
                mData.insert_slices( mData.n_slices, 1 );
                mData.slice( mData.n_slices-1 ) = pData;
                return true;
                
            } else
                
                // Don't insert
                return false;
        }
    }
    
    bool TimedCube::set( const arma::mat &pData, const double pTime ) {
		if (mIsRealTime)
			return false;
        
        if( !checkInput( pData ) ) return( false );
    
        if( mTimed ) {
            if( mTimeVec.n_elem == 0 ) return false;
 
	/*		if( mTimeVec(0) > pTime ) {
                mData.insert_slices( 0, 1 );
                mData.slice(0) = pData;
                mTimeVec.insert_rows( 0, 1 );
                mTimeVec(0) = pTime;
                return true;
            }
            
            if( mTimeVec( mTimeVec.n_elem-1 ) < pTime ) {
                mData.insert_slices( mData.n_elem, 1 );
                mData.slice( mData.n_elem-1 ) = pData;
                mTimeVec.insert_rows( mTimeVec.n_elem, 1 );
                mTimeVec( mTimeVec.n_elem-1 ) = pTime;
            	return true;
            }*/
            
			unsigned int index;
			arma::uword ltemp;
			arma::abs(this->mTimeVec - pTime).min(ltemp);
			index=(unsigned int)ltemp;
			if ( fabs(this->mTimeVec(index)-pTime ) <= 1E-6 ){
				mData.slice(index)=pData;
				return true;
			}
			if (this->mTimeVec(index)<pTime)
				index++;
	        mData.insert_slices( index, 1 );
            mData.slice( index  )= pData;
            mTimeVec.insert_rows( index, 1 );
            mTimeVec( index ) = pTime;
            return true;
        
        } else {
            
            double lTime=pTime-mInitialTime;
            if (lTime<0.0)
                return false;
            if( fabs( lTime-this->mFrameRate*round( lTime/mFrameRate ) ) <= 1E-6 ) {
                
                if( (int)round( lTime/mFrameRate ) < mData.n_slices ) {
                    
                    mData.slice( (int)round( lTime/mFrameRate ) ) = pData;
                    
                    return true;
                
                } else return false;
            
            } else return false;
        }
    }
    
    bool TimedCube::push(const arma::mat &pData, double pTime ) {
		if (!mTimed||!mIsRealTime) return false;
		if( pTime <= this->maxTime()) return false;
		mLastId++;
		if (mLastId>=mBufferSize)
			mLastId=0;
		mData.slice(mLastId)=pData;
		mTimeVec(mLastId)=pTime;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    }
    
    bool TimedCube::push(const arma::mat & pData ) {
        
		if (mTimed||!mIsRealTime) return false;
		mLastId++;
		if (mLastId>=mBufferSize)
			mLastId=0;
		mData.slice(mLastId)=pData;
		if (mIsFilled)
			mInitialTime+=1.0/mFrameRate;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    };
	
    bool TimedCube::push(const arma::cube &pData,const arma::vec &pTime){
		if (!mIsRealTime||!mTimed)
			return false;
		if (pData.n_slices!=pTime.n_elem)
			return false;
		if ((!checkTimeVec(pTime,pTime.n_elem))||(pTime(0)<=(mLastId==mBufferSize?0.0:mTimeVec(mLastId))))
			return false;
		if (pData.n_slices>=mBufferSize){
			mData=pData.slices(pData.n_slices-mBufferSize, pData.n_slices-1);
			mTimeVec=pTime.subvec(pTime.n_elem-mBufferSize, pTime.n_elem-1);
			mLastId=mBufferSize-1;
			
					mIsFilled=true;
		}
		else{
			unsigned int begInd=(mLastId+1)*((mLastId+1)<mBufferSize);
			unsigned int endInd=((mLastId+1)*((mLastId+1)<mBufferSize)+(pData.n_slices-1))%mBufferSize;
			if (begInd<=endInd){
				mData.slices(begInd,endInd)=pData;
				mTimeVec.subvec(begInd,endInd)=pTime;
				
				if (endInd==(mBufferSize-1))
					mIsFilled=true;
			}
			else{
				mData.slices(begInd,mBufferSize-1)=pData.slices(0,mBufferSize-begInd-1);
				mData.slices(0,endInd)=pData.slices(mBufferSize-begInd,pData.n_slices-1);
				mTimeVec.subvec(begInd,mBufferSize-1)=pTime.subvec(0,mBufferSize-begInd-1);
				mTimeVec.subvec(0,endInd)=pTime.subvec(mBufferSize-begInd,pTime.n_elem-1);
				
					mIsFilled=true;
			}
			mLastId=endInd;
		}

		return true;
	};
	
	bool TimedCube::push(const arma::cube &pData){
		if (!mIsRealTime||mTimed)
			return false;
		if (mIsFilled)
			mInitialTime+=(double)pData.n_cols/mFrameRate;
		else
			if (mLastId*(mLastId!=mBufferSize)+pData.n_cols>=mBufferSize)
				mInitialTime+=(double)(pData.n_cols+(1+mLastId)*(mLastId!=mBufferSize)-mBufferSize)/mFrameRate;
		if (pData.n_slices>=mBufferSize){
			mData=pData.slices(pData.n_slices-mBufferSize, pData.n_slices-1);
			mLastId=mBufferSize-1;
			
					mIsFilled=true;
		}
		else{
			unsigned int begInd=(mLastId+1)*((mLastId+1)<mBufferSize);
			unsigned int endInd=((mLastId+1)*((mLastId+1)<mBufferSize)+(pData.n_slices-1))%mBufferSize;
			if (begInd<=endInd){
				mData.slices(begInd,endInd)=pData;
				
				if (endInd==(mBufferSize-1))
					mIsFilled=true;
			}
			else{
				mData.slices(begInd,mBufferSize-1)=pData.slices(0,mBufferSize-begInd-1);
				mData.slices(0,endInd)=pData.slices(mBufferSize-begInd,pData.n_slices-1);
				
					mIsFilled=true;
			}
			mLastId=endInd;
		}

		return true;
	};
};

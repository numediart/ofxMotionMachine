//
//  mmContainer.cpp
//  MoMaBrouillon
//
//  Created by Thierry Ravet on 18/06/15.
//
//

#include "mmContainer.h"

namespace MoMa {
        
    
    void TimedData::interpIndexFind(const arma::vec pVec, double pValue, unsigned int &index1, double &weight1, unsigned int &index2, double &weight2 ){
        unsigned int lIndex1;
		arma::uword ltemp;
		arma::abs(pVec-pValue).min(ltemp);
		lIndex1=(unsigned int)ltemp;
        if ( std::abs(pVec(lIndex1)-pValue ) <= 1E-6 ){
            index1=lIndex1;
            index2=lIndex1;
            weight1=1.0;
            weight2=0.0;
            return;
        }
		if (pValue<pVec((mLastId+1)%pVec.size())){
			index1=lIndex1;
            index2=lIndex1;
            weight1=0;
            weight2=1;
			return;
		}
		if (pValue>pVec((mLastId)%pVec.size())){
			index1=lIndex1;
            index2=lIndex1;
            weight1=1;
            weight2=0;
			return;
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
		return;
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

    
    bool TimedVec::setData( double pFrameRate, const arma::vec &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
        mData = pData;
		mLastId=mData.n_elem-1;
		mBufferSize=mData.n_elem;
		mIsFilled=true;
        
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
			oneVec.setInitialTime(this->mInitialTime+usedIndex(pBegIndex)*mFrameRate);
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
		mData=initData*arma::ones(bufferSize,1);;
		mFrameRate=pFrameRate;
		mInitialTime=0.0;
		return true;
	}
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
    
    bool TimedMat::setData(  double pFrameRate, const arma::mat &pData ) {
        
        if( pFrameRate<=0.0 ) return false;
        
        mFrameRate = pFrameRate;
        this->mData = pData;
        mTimed = false;
		mLastId=mData.n_cols-1;
		mBufferSize=mData.n_cols;
		mIsFilled=true;
        
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
		mData.resize(pDim,bufferSize);
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
			oneMat.setInitialTime(this->mInitialTime+usedIndex(pBegIndex)*mFrameRate);;
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
    
    bool TimedCube::setData( double pFrameRate, const arma::cube &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
        mData = pData;
		mLastId=mData.n_slices-1;
		mBufferSize=mData.n_slices;
		mIsFilled=true;
        
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
			oneCube.setInitialTime(this->mInitialTime+usedIndex(pBegIndex)*mFrameRate);;
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
};

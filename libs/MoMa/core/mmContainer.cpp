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
    
    
    bool TimedData::checkTimeVec( arma::vec pTime ) {
        
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
    
// == TIMED VEC ==
    
    bool TimedVec::setData( float pFrameRate, const arma::vec &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
        data = pData;
        
        return true;
    }
    
    bool TimedVec::setData( const arma::vec &pTime, const arma::vec &pData ) {
        
        if( checkTimeVec(pTime) == false ) return false;
        if( pTime.n_elem != pData.n_elem ) return false;
        
        mTimeVec = pTime;
        mTimed = true;
        data = pData;
        
        return true;
    }
    
// == TIMED MAT ==
    
    bool TimedMat::setInterpAlgo( InterpTypes interpAlgo ) {
        
        this->interpolAlgo = interpAlgo;
        
        return true;
    }
    
    bool TimedMat::checkInput( const arma::vec &pData ) {
        
        if( data.n_elem > 0 && pData.n_rows != data.n_rows ) return false;
        else return true;
    }
    
    bool TimedMat::setData( const float pFrameRate, const arma::mat &pData ) {
        
        if( pFrameRate<=0.0 ) return false;
        
        mFrameRate = pFrameRate;
        this->data = pData;
        mTimed = false;
        
        return true;
    }
    
    bool TimedMat::setData( const arma::vec &pTime, const arma::mat &pData ) {
        
        if( checkTimeVec(pTime) == false ) return false;
        if( pTime.n_elem != pData.n_cols ) return false;
        
        mTimeVec = pTime;
        mTimed = true;
        data = pData;
        
        return true;
    }
    
// == TIMED CUBE ==
    
    bool TimedCube::setInterpAlgo( InterpTypes interpAlgo ) {
        
        this->interpolAlgo = interpAlgo;
        
        return( true );
    }
    
    bool TimedCube::checkInput( const arma::mat &pData ) {
        
        if( data.n_elem > 0 && ( pData.n_rows != data.n_rows
        || pData.n_cols != data.n_cols ) ) return( false );
        else return( true );
    }
    
    bool TimedCube::setData( float pFrameRate, const arma::cube &pData ) {
        
        if( pFrameRate <= 0.0 ) return false;
        
        mFrameRate = pFrameRate;
        mTimed = false;
        data = pData;
        
        return( true );
    }
    
    bool TimedCube::setData( const arma::vec &pTime, const arma::cube &pData ) {
        
        if ( checkTimeVec( pTime ) == false ) return false;
        if ( pTime.n_elem != pData.n_slices ) return false;
        
        mTimeVec = pTime;
        mTimed = true;
        data = pData;
        
        return( true );
    }
};

#ifndef __mmContainer__
#define __mmContainer__

#include <iostream>
#include <armadillo>
#ifdef WIN32 
#define round(a) (a < 0.0 ? ceil(a - 0.5) : floor(a + 0.5))
#endif
inline unsigned int umin(unsigned int a, unsigned int b) { return a<b ? a : b; }
inline unsigned int umax(unsigned int a, unsigned int b) { return a>b ? a : b; }
namespace MoMa {
    
    /**
     * A class that provides a way to store one double value for each frame.
     * The timedflag precises if the framerate is fixed or if we use a timestamp
     * vector to define the occuring time for each frame in the container.
     */
    
    enum InterpTypes {
        
        STEP, // Step interpolation
        LINEAR, // Linear interpolation
        QSLERP // Qslerp interpolation
        
        // TODO Implement 'QSLERP'
    };
    
    class interpolAlgo{
    public:
        virtual arma::Mat<double> apply(double tCurr,arma::Mat<double> v1,double t1,arma::Mat<double> v2,double t2)=0;
    };
    
    class stepInterpol: public interpolAlgo{
        arma::Mat<double> apply(double tCurr,arma::Mat<double> v1,double t1,arma::Mat<double> v2,double t2);
    };
    
    class linearInterpol: public interpolAlgo{
        arma::Mat<double> apply(double tCurr,arma::Mat<double> v1,double t1,arma::Mat<double> v2,double t2);
    };
    
    class qslerp: public interpolAlgo{
        arma::Mat<double> apply(double tCurr,arma::Mat<double> v1,double t1,arma::Mat<double> v2,double t2);
    };
    // TODO implement conversion from timestamped mode to fixed
    // framerate with linear arma::interp1 & qslerp interpolation
    
    // == TIMED DATA ==
    
    class TimedData {
        
      protected:
        
        TimedData() { mTimed = false; mFrameRate = -1.0f;mInitialTime=0.f; mTimeVec=arma::zeros(0,0);
		mBufferSize=0;mLastId=0;mIsRealTime=false;mBufferSize=0;mLastId=0;mIsFilled=true;}
	public:
        inline const arma::vec &getTimeVec( void ) const{ return( mTimeVec ); }
        inline arma::vec &getTimeVecRef( void ){ return( mTimeVec ); }
        inline bool isTimed( void ) const { return( mTimed ); }
        
        inline double time( unsigned int index ) const;
        inline unsigned int nearestIndex( double time ) const;
        inline double maxTime( void ) const; // Time getters
        inline double minTime( void ) const; // Time getters
        
        virtual unsigned int nOfFrames( void ) const { return( 0 ); }
        inline bool setFrameRate( double pFrameRate );
        inline bool setInitialTime( double pInitialTime );
        inline double frameRate( void ) const; // Frame
        inline double initialTime( void ) const; // Frame
        
		//RealTime methods
		inline bool isRealTime() const{return mIsRealTime;} ;
		inline unsigned int getBufferSize(){return mBufferSize;};
		inline unsigned int setBufferSize(unsigned int pBufferSize){mBufferSize=pBufferSize;};
		inline unsigned int memIndex(unsigned int index) const;//transform the index relative to mLastId in the index in memory
		inline unsigned int usedIndex(unsigned int index) const;
		void setIsFilled(bool pIsFilled){
			mIsFilled=pIsFilled;
		}
		void setLastId(unsigned int lId){
			mLastId=lId;
		}
		virtual bool SetValidParam()=0;//Check if size and flag is ok and make change if necessary
      protected:
        
        bool checkTimeVec( arma::vec pTime ); // Check time stamp vector
        unsigned int checkLastId(const arma::vec &pTime);
        void interpIndexFind( const arma::vec pVec, double pValue, unsigned int &index1,
        double &weight1, unsigned int &index2, double &weight2 ); // Interpolation data
        
    protected:
        arma::vec mTimeVec; // Time stamp vector
        double mFrameRate; // Frame rate (if any)
        double mInitialTime;
        bool mTimed; // Is it timed or not?
		
		//RealTime properties
		void initRealTime(unsigned int bufferSize){
			mTimeVec.clear();
			mIsRealTime=true;
			mBufferSize=bufferSize;
			mIsFilled=false;
			mLastId=mBufferSize;//initValue when the ring buffer is empty
		}
		bool mIsRealTime;
		unsigned int mBufferSize;
		bool mIsFilled;
		unsigned int mLastId;//=mBufferSize-1 if mIsRealTime==false
    };
    
    // - Inlined functions -
	unsigned int TimedData::memIndex(unsigned int index) const {
		return (mIsFilled?((int)index+1+mLastId)%mBufferSize:umin(index,mLastId%mBufferSize));
	}
	
	unsigned int TimedData::usedIndex(unsigned int index) const {
		return (mIsFilled?((int)mBufferSize+index-1-mLastId)%mBufferSize:umin(index,mLastId%mBufferSize));
	}
    double TimedData::time( unsigned int index ) const{
        
        // Return/compute time at current index
		if( mTimed ) return( mTimeVec[ memIndex(index) ] );
        else return( mInitialTime+( (double)index  ) / mFrameRate );
    }
    
    unsigned int TimedData::nearestIndex( double time ) const{
        if (!mTimed){
            int lIndex=(int)(0.5+(time-mInitialTime)*mFrameRate);
			if (lIndex<0) lIndex=0;
			if (lIndex>=mBufferSize) lIndex=mBufferSize-1;
            return usedIndex(lIndex);
        }
        else{
			unsigned int lIndex;
			arma::uword ltemp;
			arma::abs(mTimeVec-time).min(ltemp);
			lIndex=(unsigned int)ltemp;
            return usedIndex(lIndex);
        }
    }
    double TimedData::maxTime( void ) const {
        
        // Return/compute time at the last index of time
        if( mTimed ) return( mTimeVec.max() );
		else return ( mIsFilled? (mInitialTime+(double)( mBufferSize-1 )  / mFrameRate ):(mInitialTime+(double)(mLastId%mBufferSize)  / mFrameRate));
	}
    double TimedData::minTime( void ) const{
        // Return/compute time at the last index of time
		if( mTimed ) return (mIsFilled?mTimeVec.min(): mTimeVec.at(0) );
        else return(  mInitialTime);
    }
    
    bool TimedData::setFrameRate( double pFrameRate ) {
        
        if( mTimed ) return( false ); // Set frame rate
        else { mFrameRate = pFrameRate; return( true ); }
    }
    
    bool TimedData::setInitialTime( double pInitialTime ) {
        
        if( mTimed ) return( false ); // Set frame rate
        else { mInitialTime = pInitialTime; return( true ); }
    }
    
    double TimedData::frameRate( void ) const {
        
        // Get frame rate ( if any )
        return( mTimed?-1.0f:mFrameRate );
    }
    
    double TimedData::initialTime( void ) const {
        
        // Get Initial Time ( if any )
        return( mTimed?-1.0f:mInitialTime );
    }
    
    // == TIMED VEC ==
    
    class TimedVec : public TimedData {
        
      public:
        inline double get( unsigned int pIndex );
        inline double get( double pTime );
        
        inline bool set( double pData, unsigned int pIndex );
        inline bool set( double pData, double pTime );
        
        inline bool push( double pData, double pTime );
        inline bool push( double pData );
        
        TimedVec sub( int pBegIndex, int pEndIndex ) const;
        TimedVec getOfflineData( ) const;
        
        bool setRealTimeMode(unsigned int bufferSize,double initData);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,double initData);
		bool setData( double pFrameRate, const arma::vec &pData );
        bool setData( arma::vec const &pTime, const arma::vec &pData );
        bool swapData( double pFrameRate,  arma::vec &pData );
        bool swapData( arma::vec  &pTime,  arma::vec &pData );
        
        inline const arma::vec &getData( void ) const { return mData; }
        inline arma::vec &getRefData( void ) { return mData; }
        
		unsigned int nOfFrames( void ) const { return( mIsFilled?mData.n_elem:(mLastId+1)*(mLastId<mBufferSize) ); }
        unsigned int nOfElems( void ) const { return( 1 ); }
        void clear( void ) { mData.clear(); }
		bool SetValidParam();
      protected:
        
        arma::vec mData;
    };
    
    // - Inlined functions -
    
	
    double TimedVec::get( unsigned int pIndex ) {
        
        return mData( memIndex(pIndex) );
    }
    
    double TimedVec::get( double pTime ) {
        
        if( mTimed ) {
            
            double weight1, weight2;
            unsigned int index1, index2;
            
            interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            
            return( ( weight1*mData(index1)+weight2*mData(index2))/(weight1+weight2) );
        
        } else {
			return( this->mData(memIndex( this->nearestIndex(pTime)) ));
        }
        
        return( 0.0 );
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
			if ( std::abs(this->mTimeVec(index)-pTime ) <= 1E-6 ){
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
            if( std::abs( (lTime-mFrameRate*round(lTime/mFrameRate)) ) <= 1E-6 ) {
                
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

    
   // void TimedVec::pop( void ) {
        
   //     mData.shed_col( 0 ); // Shed one value
   //     if( mTimed ) mTimeVec.shed_row( 0 );
   // }
    
    
    // == TIMED MAT ==
    
    class TimedMat: public TimedData {
        
      public:
        
        
        TimedMat( void ) { interpolAlgo = LINEAR; }
        
        inline arma::vec get( unsigned int pIndex ); // Frame getter
        inline arma::vec get( double pTime ); // by index and time
        
        inline bool set( const arma::vec &pData, unsigned int pIndex ); // Frame setter
        inline bool set( const arma::vec &pData, double pTime ); // by index and time
        
        inline bool push( const arma::vec &pData, double pTime ); // Push timed
        inline bool push( const arma::vec &pData ); // Push at the end of mData
        
        TimedMat sub( int pBegIndex, int pEndIndex ) const; // Chop by index
        TimedMat getOfflineData( ) const;
        TimedVec elem( unsigned int pIndex ) const; // Elem getter by index
        
        bool setRealTimeMode(unsigned int bufferSize,arma::vec initData);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,arma::vec initData);
        bool setRealTimeMode(unsigned int bufferSize,unsigned int pDim);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,unsigned int pDim);
        bool setData( double pFrameRate, const arma::mat &pData );
        bool setData(const arma::vec  &pTime, const arma::mat &pData );
        bool swapData( double pFrameRate,  arma::mat &pData ); // Set indexed
        bool swapData(  arma::vec &pTime,  arma::mat &pData ); // Set timed
        
        inline const arma::mat &getData( void ) const { return mData; } // Get mData copy
        inline arma::mat &getRefData( void ) { return mData; } // Get mData ref
        
        bool setInterpAlgo( InterpTypes interpAlgo );
        unsigned int nOfFrames( void ) const { return( mIsFilled?mData.n_cols:((mLastId+1)*(mLastId<mBufferSize))); }
        unsigned int nOfElems( void ) const { return( mData.n_rows ); }
        void clear( void ) { mData.clear(); }
		bool SetValidParam();
    
      protected:
        
        bool checkInput( const arma::vec &pData );
        
        InterpTypes interpolAlgo;
        arma::mat mData;
    };
    
    // - Inlined functions -
    
    arma::vec TimedMat::get( unsigned int pIndex ) {
        
		return mData.col( memIndex( pIndex) );
    }
    
    arma::vec TimedMat::get( double pTime ) {
        
        if( mTimed ) {
            
            double weight1,weight2;
            unsigned int index1,index2;
            
            interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            return( ( weight1*mData.col(index1) + weight2*mData.col(index2) ) / (weight1+weight2) );
            
            // TODO : Implement QSLERP interpolation
        
        } else 
			return ( this->mData.col(memIndex( this->nearestIndex(pTime)) ));
        
    }
    
    bool TimedMat::set( const arma::vec &pData, unsigned int pIndex ) {
        
		if (mIsRealTime)
			return false;
        if( mTimed ) return false;
        if( !checkInput(pData) ) return false;
        
		pIndex=memIndex(pIndex);
        
        if( pIndex<mData.n_elem ) {
            
            mData.col( pIndex ) = pData;
            
            return true;
        
        } else {
            
            if( pIndex == mData.n_elem ) {
                
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
			if ( std::abs(this->mTimeVec(index)-pTime ) <= 1E-6 ){
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
            if( std::abs( lTime-mFrameRate*round(lTime/mFrameRate) ) <= 1E-6 ) {
                
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
			mLastId=0;;
		mData.col(mLastId)=pData;
		if (mIsFilled)
			mInitialTime+=1.0/mFrameRate;
		if (mLastId==mBufferSize-1)
			mIsFilled=true;
        return true;
    };
    
    
    
    // == TIMED CUBE ==
    
    class TimedCube: public TimedData {
        
      public:
        
        TimedCube( void ) { interpolAlgo = LINEAR; }
        
        inline const arma::mat &get( unsigned int pIndex ); // Frame getter
        inline arma::mat get( double pTime ); // by index and time
        
        inline bool set( const arma::mat &pData, unsigned int pIndex ); // Frame setter
        inline bool set( const arma::mat &pData, double pTime ); // by index and time
        
        inline bool push( const arma::mat &pData, double pTime ); // Push timed
        inline bool push( const arma::mat &pData ); // Push at the end of mData
        
        TimedCube sub( int pBegIndex, int pEndIndex ) const; // Chop by index
        TimedCube getOfflineData( ) const;
        TimedMat row( unsigned int pIndex ); // Row getter by index
        TimedMat col( unsigned int pIndex ); // Col getter by index
        
        bool setRealTimeMode(unsigned int bufferSize,arma::mat initData);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,arma::mat initData);
        bool setRealTimeMode(unsigned int bufferSize,unsigned int pDim0,unsigned int pDim1);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,unsigned int pDim0,unsigned int pDim1);

        bool setData( double pFrameRate, const arma::cube &pData ); // Set indexed
        bool setData( const arma::vec &pTime, const arma::cube &pData ); // Set timed
        bool swapData( double pFrameRate,  arma::cube &pData ); // Set indexed
        bool swapData(  arma::vec &pTime,  arma::cube &pData ); // Set timed
        
        inline const arma::cube &getData( void ) const{ return mData; } // Get mData copy
        inline arma::cube &getRefData( void ) { return mData; } // Get mData ref
        
        bool setInterpAlgo( InterpTypes interpAlgo ); // Set interp algo
        unsigned int nOfFrames( void ) const { return( mIsFilled?mData.n_slices:((mLastId+1)*(mLastId<mBufferSize)) ); } // # of frames
        unsigned int nOfRows( void ) const { return( mData.n_rows ); } // # of rows
        unsigned int nOfCols( void ) const { return( mData.n_cols ); } // # of cols
        void clear( void ) { mData.clear(); } // Clear
 		bool SetValidParam();
       
      protected:
        
        bool checkInput( const arma::mat &pData );
        
        InterpTypes interpolAlgo;
        arma::cube mData;
    };
    
    // - Inlined functions -
    
    const arma::mat &TimedCube::get( unsigned int pIndex ) {
        
		return mData.slice( memIndex( pIndex) );
    }
    
    arma::mat TimedCube::get( double pTime ) {
        
        if( mTimed ) {
            
            double weight1,weight2;
            unsigned int index1,index2;
            
            interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            
            if( interpolAlgo == LINEAR ) {
                
                return( ( weight1 * mData.slice( index1 ) + weight2
                         * mData.slice( index2 ) )/ ( weight1+weight2 ) );
                
            } else if ( interpolAlgo == QSLERP ) {
                
                // TODO : Implement QSLERP interpolation
                return arma::zeros( 1, 1 );
                
            } else {
                
                return arma::zeros( 1, 1 );
            }
            
        } else {
			return ( this->mData.slice(memIndex( this->nearestIndex(pTime)) ));
        }
        
        return arma::zeros( 1, 1 );
    }
    
    bool TimedCube::set( const arma::mat &pData, unsigned int pIndex ) {
		if (mIsRealTime)
			return false;
        if( mTimed ) return false;
        if( !checkInput( pData ) ) return false;
        // -> No go check: not timed or bad format
        
        pIndex=memIndex(pIndex);
        
        if( pIndex < mData.n_elem ) {
            
            // Insert mData into cube
            mData.slice( pIndex ) = pData;
            return true;
            
        } else {
            
            if( pIndex == mData.n_elem ) {
                
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
			if ( std::abs(this->mTimeVec(index)-pTime ) <= 1E-6 ){
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
            if( std::abs( lTime-this->mFrameRate*round( lTime/mFrameRate ) ) <= 1E-6 ) {
                
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
	
};

#endif

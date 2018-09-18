#ifndef __mmContainer__
#define __mmContainer__

#include <iostream>
#include <armadillo>
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
        
          TimedData() {
              mTimed = false; mFrameRate = -1.0f; mInitialTime = 0.f; mTimeVec = arma::zeros( 0, 0 ); mInterpValidTime = 0.5;
		mBufferSize=0;mLastId=0;mIsRealTime=false;mBufferSize=0;mLastId=0;mIsFilled=true;}
	public:
        inline const arma::vec &getTimeVec( void ) const{ return( mTimeVec ); }
        inline arma::vec &getTimeVecRef( void ){ return( mTimeVec ); }
        inline bool isTimed( void ) const { return( mTimed ); }
		inline unsigned int lastId() const {return (mIsFilled?((int)mBufferSize-1):mLastId);}
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
		inline unsigned int getBufferSize() const{return mBufferSize;};
		inline void setBufferSize(unsigned int pBufferSize){mBufferSize=pBufferSize;};
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
        bool checkTimeVec( arma::vec pTime ,unsigned int pLastId); // Check time stamp vector
        unsigned int checkLastId(const arma::vec &pTime);
        bool interpIndexFind( const arma::vec pVec, double pValue, unsigned int &index1,
        double &weight1, unsigned int &index2, double &weight2 ) const; // Interpolation data
        void setValidIntervalTime( double time ) { mInterpValidTime = time; };

    protected:
        arma::vec mTimeVec; // Time stamp vector
        double mFrameRate; // Frame rate (if any)
        double mInitialTime;
        bool mTimed; // Is it timed or not?
        double mInterpValidTime;
		
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
            return (lIndex);
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

        TimedVec() : TimedData() {};
        TimedVec(double pFrameRate, const arma::vec &pData, float initTime = 0.0f) : TimedVec() { setData(pFrameRate, pData, initTime); };
        TimedVec(const arma::vec  &pTime, const arma::vec &pData) : TimedVec() { setData(pTime, pData); };

        inline double get( unsigned int pIndex );
        inline double get( double pTime );
		
		inline double getLast( double &pTime );
		inline double getLast( );

         bool set( double pData, unsigned int pIndex );
         bool set( double pData, double pTime );
        
         bool push( double pData, double pTime );
         bool push( double pData );

		 bool push(const arma::vec &pData,const arma::vec &pTime);
		 bool push(const arma::vec &pData);

        TimedVec sub( int pBegIndex, int pEndIndex ) const;
        TimedVec getOfflineData( ) const;
        
        bool setRealTimeMode(unsigned int bufferSize,double initData);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,double initData);
		bool setData( double pFrameRate, const arma::vec &pData, float initTime = 0.0f );
        bool setData( arma::vec const &pTime, const arma::vec &pData );
        bool swapData( double pFrameRate,  arma::vec &pData );
        bool swapData( arma::vec  &pTime,  arma::vec &pData );
        
        inline const arma::vec &getData( void ) const { return mData; }
        inline arma::vec &getRefData( void ) { return mData; }
        
		unsigned int nOfFrames( void ) const { return( mIsFilled?mData.n_elem:(mLastId+1)*(mLastId<mBufferSize) ); }
        unsigned int nOfElems( void ) const { return( 1 ); }
        void clear( void ) { mData.clear(); }
		bool SetValidParam();

        TimedVec operator+(double d) { TimedVec ret(*this); ret.getRefData() += d; return ret; }
        TimedVec operator-(double d) { TimedVec ret(*this); ret.getRefData() -= d; return ret; }
        TimedVec operator*(double d) { TimedVec ret(*this); ret.getRefData() *= d; return ret; }
        TimedVec operator/(double d) { TimedVec ret(*this); ret.getRefData() /= d; return ret; }
        void operator+=(double d) { this->getRefData() += d;}
        void operator-=(double d) { this->getRefData() -= d; }
        void operator*=(double d) { this->getRefData() *= d; }
        void operator/=(double d) { this->getRefData() /= d; }
	
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
            
            bool validFlag=interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            if( validFlag )
                return( ( weight1*mData( index1 ) + weight2*mData( index2 ) ) / ( weight1 + weight2 ) );
            else
                return arma::datum::nan;
        
        } else {
			return( this->mData(memIndex( this->nearestIndex(pTime)) ));
        }
        
        return( 0.0 );
    }
		
	double TimedVec::getLast( double &pTime ){
		if (mLastId>=mBufferSize){
			pTime=-1.0;
			return 0.0;
		}
		else{
			if (mTimed)
				pTime=mTimeVec(mLastId);
			else
				pTime=mInitialTime+mLastId/mFrameRate;
			return 
				mData(mLastId);
		}
	};

	double TimedVec::getLast( ){
		if (mLastId>=mBufferSize)
			return 0.0;
		else{
			return 
				mData(mLastId);
		}
	};
    
    
   // void TimedVec::pop( void ) {
        
   //     mData.shed_col( 0 ); // Shed one value
   //     if( mTimed ) mTimeVec.shed_row( 0 );
   // }
    
    
    // == TIMED MAT ==
    
    class TimedMat: public TimedData {
        
      public:
        
        
        TimedMat( void ) { interpolAlgo = LINEAR; }
        TimedMat(double pFrameRate, const arma::mat &pData, float initTime = 0.0f) : TimedMat() { setData(pFrameRate, pData, initTime); };
        TimedMat(const arma::vec  &pTime, const arma::mat &pData) : TimedMat() { setData(pTime, pData); };

        inline arma::vec get( unsigned int pIndex ); // Frame getter
        inline arma::vec get( double pTime ); // by index and time
        
		inline arma::vec getLast( double &pTime );
		inline arma::vec getLast( ); 
         bool set( const arma::vec &pData, unsigned int pIndex ); // Frame setter
         bool set( const arma::vec &pData, double pTime ); // by index and time
        
         bool push( const arma::vec &pData, double pTime ); // Push timed
         bool push( const arma::vec &pData ); // Push at the end of mData
        
		 bool push(const arma::mat &pData,const arma::vec &pTime);
		 bool push(const arma::mat &pData);

        TimedMat sub( int pBegIndex, int pEndIndex ) const; // Chop by index
        TimedMat getOfflineData( ) const;
        TimedVec elem( unsigned int pIndex ) const; // Elem getter by index
        
        bool setRealTimeMode(unsigned int bufferSize,arma::vec initData);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,arma::vec initData);
        bool setRealTimeMode(unsigned int bufferSize,unsigned int pDim);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,unsigned int pDim);
        bool setData( double pFrameRate, const arma::mat &pData, float initTime = 0.0f);
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

        TimedMat operator+(double d) { TimedMat ret(*this); ret.getRefData() += d; return ret; }
        TimedMat operator-(double d) { TimedMat ret(*this); ret.getRefData() -= d; return ret; }
        TimedMat operator*(double d) { TimedMat ret(*this); ret.getRefData() *= d; return ret; }
        TimedMat operator/(double d) { TimedMat ret(*this); ret.getRefData() /= d; return ret; }
        void operator+=(double d) { this->getRefData() += d; }
        void operator-=(double d) { this->getRefData() -= d; }
        void operator*=(double d) { this->getRefData() *= d; }
        void operator/=(double d) { this->getRefData() /= d; }
    
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
            
            bool validFlag=interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            if( validFlag )
                return ( ( weight1*mData.col( index1 ) + weight2*mData.col( index2 ) ) / ( weight1 + weight2 ) );
            else
                return (arma::datum::nan*arma::ones( mData.n_rows, 1 ));
            
            // TODO : Implement QSLERP interpolation
        
        } else 
			return ( this->mData.col(memIndex( this->nearestIndex(pTime)) ));
        
    }
    	
	arma::vec TimedMat::getLast( double &pTime ){
		if (mLastId>=mBufferSize){
			pTime=-1.0;
			return arma::zeros(mData.n_cols,1);
		}
		else{
			if (mTimed)
				pTime=mTimeVec(mLastId);
			else
				pTime=mInitialTime+mLastId/mFrameRate;
			return 
				mData.col(mLastId);
		}
	};

	arma::vec TimedMat::getLast( ){
		if (mLastId>=mBufferSize)
			return arma::zeros(mData.n_cols,1);
		else{
			return 
				mData.col(mLastId);
		}
	};
    
    
    // == TIMED CUBE ==
    
    class TimedCube: public TimedData {
        
      public:
        
        TimedCube( void ) { interpolAlgo = LINEAR; }
        TimedCube(double pFrameRate, const arma::cube &pData, float initTime = 0.0f) : TimedCube() { setData(pFrameRate, pData, initTime); };
        TimedCube(const arma::vec  &pTime, const arma::cube &pData) : TimedCube() { setData(pTime, pData); };
        
        inline const arma::mat &get( unsigned int pIndex ) const; // Frame getter
        inline arma::mat get( double pTime ) const; // by index and time
        
		inline arma::mat getLast( double &pTime );
		inline arma::mat getLast( ); 

         bool set( const arma::mat &pData, unsigned int pIndex ); // Frame setter
         bool set( const arma::mat &pData, double pTime ); // by index and time
        
         bool push( const arma::mat &pData, double pTime ); // Push timed
         bool push( const arma::mat &pData ); // Push at the end of mData
        
		 bool push(const arma::cube &pData,const arma::vec &pTime);
		 bool push(const arma::cube &pData);

		 
		TimedCube sub(double pBegTime, double pEndTime) const;
        TimedCube sub( int pBegIndex, int pEndIndex ) const; // Chop by index
        TimedCube getOfflineData( ) const;
        TimedMat row( unsigned int pIndex ); // Row getter by index
        TimedMat col( unsigned int pIndex ); // Col getter by index
        
        bool setRealTimeMode(unsigned int bufferSize,arma::mat initData);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,arma::mat initData);
        bool setRealTimeMode(unsigned int bufferSize,unsigned int pDim0,unsigned int pDim1);
		bool setRealTimeMode(unsigned int bufferSize,double pFrameRate,unsigned int pDim0,unsigned int pDim1);

        bool setData( double pFrameRate, const arma::cube &pData, float initTime = 0.0f); // Set indexed
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

        TimedCube operator+(double d) { TimedCube ret(*this); ret.getRefData() += d; return ret; }
        TimedCube operator-(double d) { TimedCube ret(*this); ret.getRefData() -= d; return ret; }
        TimedCube operator*(double d) { TimedCube ret(*this); ret.getRefData() *= d; return ret; }
        TimedCube operator/(double d) { TimedCube ret(*this); ret.getRefData() /= d; return ret; }
        void operator+=(double d) { this->getRefData() += d; }
        void operator-=(double d) { this->getRefData() -= d; }
        void operator*=(double d) { this->getRefData() *= d; }
        void operator/=(double d) { this->getRefData() /= d; }
       
      protected:
        
        bool checkInput( const arma::mat &pData );
        
        InterpTypes interpolAlgo;
        arma::cube mData;
    };
    
    // - Inlined functions -
    
    const arma::mat &TimedCube::get( unsigned int pIndex ) const{
        
		return mData.slice( memIndex( pIndex) );
    }
    
    arma::mat TimedCube::get( double pTime ) const {
        
        if( mTimed ) {
            
            double weight1,weight2;
            unsigned int index1,index2;
            
            bool validFlag =interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );

            if( interpolAlgo == LINEAR ) {
                if( validFlag )  
                    return( ( weight1 * mData.slice( index1 ) + weight2
                         * mData.slice( index2 ) )/ ( weight1+weight2 ) );
                else
                    return ( arma::datum::nan*arma::ones( mData.n_rows, mData.n_cols ) );

                
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
    
	arma::mat TimedCube::getLast( double &pTime ){
		if (mLastId>=mBufferSize){
			pTime=-1.0;
			return arma::zeros(mData.n_cols,mData.n_rows);
		}
		else{
			if (mTimed)
				pTime=mTimeVec(mLastId);
			else
				pTime=mInitialTime+mLastId/mFrameRate;
			return 
				mData.slice(mLastId);
		}
	};

	arma::mat TimedCube::getLast( ){
		if (mLastId>=mBufferSize)
			return arma::zeros(mData.n_cols,mData.n_rows);
		else{
			return 
				mData.slice(mLastId);
		}
	};
  
	
};


#endif

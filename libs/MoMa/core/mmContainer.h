#ifndef __mmContainer__
#define __mmContainer__

#include <iostream>
#include <armadillo>

#ifdef WIN32 
#define round(a) (a < 0.0 ? ceil(a - 0.5) : floor(a + 0.5))
#endif

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
    
    // TODO implement conversion from timestamped mode to fixed
    // framerate with linear arma::interp1 & qslerp interpolation
    
    // == TIMED DATA ==
    
    class TimedData {
        
      public:
        
        TimedData( void ) { mTimed = false; mFrameRate = 1.0f; }
        
        inline const arma::vec &getTimeVec( void ){ return( mTimeVec ); }
        inline arma::vec &getTimeVecRef( void ){ return( mTimeVec ); }
        inline bool isTimed( void ){ return( mTimed ); }
        
        inline double time( unsigned int index );
        inline double maxTime( void ); // Time getters
        
        virtual unsigned int nOfFrames( void ) { return( 0 ); }
        inline bool setFrameRate( float pFrameRate );
        inline float frameRate( void ); // Frame
        
      protected:
        
        bool checkTimeVec( arma::vec pTime ); // Check time stamp vector
        
        void interpIndexFind( const arma::vec pVec, double pValue, unsigned int &index1,
        double &weight1, unsigned int &index2, double &weight2 ); // Interpolation data
        
        arma::vec mTimeVec; // Time stamp vector
        float mFrameRate; // Frame rate (if any)
        bool mTimed; // Is it timed or not?
    };
    
    // - Inlined functions -
    
    double TimedData::time( unsigned int index ) {
        
        // Return/compute time at current index
        if( mTimed ) return( mTimeVec[ index ] );
        else return( ( (double)index ) / mFrameRate );
    }
    
    double TimedData::maxTime( void ) {
        
        // Return/compute time at the last index of time
        if( mTimed ) return( mTimeVec[ mTimeVec.n_elem-1 ] );
        else if( nOfFrames() > 0 ) return( ( (double)( nOfFrames()-1 ) ) / mFrameRate );
        else return( 0.0f ); // We make sure that an empty container is maxTime = 0
    }
    
    bool TimedData::setFrameRate( float pFrameRate ) {
        
        if( mTimed ) return( false ); // Set frame rate
        else { mFrameRate = pFrameRate; return( true ); }
    }
    
    float TimedData::frameRate( void ) {
        
        // Get frame rate ( if any )
        return( mTimed?-1.0f:mFrameRate );
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
        inline void pop( void );
        
        inline TimedVec sub( int pBegIndex, int pEndIndex );
        
        bool setData( float pFrameRate, const arma::vec &pData );
        bool setData( arma::vec const &pTime, const arma::vec &pData );
        
        inline const arma::vec &getData( void ) { return data; }
        inline arma::vec &getRefData( void ) { return data; }
        
        unsigned int nOfFrames( void ) { return( data.n_elem ); }
        unsigned int nOfElems( void ) { return( 1 ); }
        void clear( void ) { data.clear(); }
        
      protected:
        
        arma::vec data;
    };
    
    // - Inlined functions -
    
    double TimedVec::get( unsigned int pIndex ) {
        
        return data( pIndex );
    }
    
    double TimedVec::get( double pTime ) {
        
        if( mTimed ) {
            
            double weight1, weight2;
            unsigned int index1, index2;
            
            interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            return( ( weight1*data(index1)+weight2*data(index2))/(weight1+weight2) );
        
        } else {
            
            return( this->data( (int)(pTime*mFrameRate) ) );
        }
        
        return( 0.0 );
    }
    
    bool TimedVec::set( double pData, unsigned int pIndex ) {
        
        if( mTimed ) return false;
        
        if( pIndex < data.n_elem ) {
            
            data(pIndex) = pData;
            return true;
        
        } else {
            
            if( pIndex == data.n_elem ) {
                
                data.insert_rows( data.n_elem, 1 );
                data( data.n_elem-1 ) = pData;
                
                return true;
            
            }
            
            else return false;
        }
        
        return true;
    }
    
    bool TimedVec::set( double pData, double pTime ) {
        
        if( mTimed ) {
            
            if( mTimeVec.n_elem == 0 ) return false;
            
            if( mTimeVec(0) > pTime ) {
                
                data.insert_rows( 0, 1 );
                data(0) = pData;
                mTimeVec.insert_rows( 0, 1 );
                mTimeVec(0) = pTime;
                
                return true;
            }
            
            if( mTimeVec( mTimeVec.n_elem-1 ) < pTime ) {
                
                data.insert_rows( data.n_elem, 1 );
                data( data.n_elem-1 ) = pData;
                mTimeVec.insert_rows( mTimeVec.n_elem, 1 );
                mTimeVec( mTimeVec.n_elem-1 ) = pTime;
                
                return true;
            }
            
            arma::uvec index = find( this->mTimeVec == pTime );
            
            if( index.n_elem > 0 ) {
                
                data( index(0) ) = pData;
                
                return true;
            
            } else {
                
                index = find( this->mTimeVec > pTime );
                
                if( index.n_elem > 0 ) {
                    
                    data.insert_rows( index(0), 1 );
                    data( index(0)  )= pData;
                    mTimeVec.insert_rows( index(0), 1 );
                    mTimeVec( index(0) ) = pTime;
                    
                    return true;
                
                } else return false;
            }
        
        } else {
            
            if( std::abs( (pTime-mFrameRate*round(pTime/mFrameRate)) ) <= 1E-6 ) {
                
                if( (int)round(pTime/mFrameRate) < data.n_elem ) {
                    
                    data( (int)round(pTime/mFrameRate) ) = pData;
                    
                    return true;
                
                } else return false;
            
            } else return false;
        }
    }
    
    bool TimedVec::push( double pData, double pTime ) {
        
        if( pTime < this->mTimeVec( mTimeVec.n_elem-1 ) ) return false;
        this->set( pData, pTime );
        
        return true;
    }
    
    bool TimedVec::push( double pData ) {
        
        if( mTimed ) return false;
        
        data.insert_rows( data.n_elem, 1 );
        data( data.n_elem-1 ) = pData;
        
        return true;
    };
    
    void TimedVec::pop( void ) {
        
        data.shed_col( 0 ); // Shed one value
        if( mTimed ) mTimeVec.shed_row( 0 );
    }
    
    TimedVec TimedVec::sub( int pBegIndex, int pEndIndex ) {
    
        TimedVec oneVec = *this;
        
        oneVec.data = data.subvec( pBegIndex, pEndIndex ); // Chopping data vector and timestamps
        if( isTimed() ) oneVec.getTimeVecRef() = getTimeVecRef().subvec( pBegIndex, pEndIndex );
        
        return( oneVec );
    }
    
    // == TIMED MAT ==
    
    class TimedMat: public TimedData {
        
      public:
        
        TimedMat( void ) { interpolAlgo = LINEAR; }
        
        inline arma::vec get( unsigned int pIndex ); // Frame getter
        inline arma::vec get( double pTime ); // by index and time
        
        inline bool set( const arma::vec &pData, unsigned int pIndex ); // Frame setter
        inline bool set( const arma::vec &pData, double pTime ); // by index and time
        
        inline bool push( const arma::vec &pData, double pTime ); // Push timed
        inline bool push( const arma::vec &pData ); // Push at the end of data
        inline void pop( void ); // Pop oldest frame from the data container
        
        inline TimedMat sub( int pBegIndex, int pEndIndex ); // Chop by index
        inline TimedVec elem( unsigned int pIndex ); // Elem getter by index
        
        bool setData( float pFrameRate, const arma::mat &pData ); // Set indexed
        bool setData( const arma::vec &pTime, const arma::mat &pData ); // Set timed
        
        inline const arma::mat &getData( void ) { return data; } // Get data copy
        inline arma::mat &getRefData( void ) { return data; } // Get data ref
        
        bool setInterpAlgo( InterpTypes interpAlgo );
        unsigned int nOfFrames( void ) { return( data.n_cols ); }
        unsigned int nOfElems( void ) { return( data.n_rows ); }
        void clear( void ) { data.clear(); }
    
      protected:
        
        bool checkInput( const arma::vec &pData );
        
        InterpTypes interpolAlgo;
        arma::mat data;
    };
    
    // - Inlined functions -
    
    arma::vec TimedMat::get( unsigned int pIndex ) {
        
        return data.col( pIndex );
    }
    
    arma::vec TimedMat::get( double pTime ) {
        
        if( mTimed ) {
            
            double weight1,weight2;
            unsigned int index1,index2;
            
            interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            return( ( weight1*data.col(index1) + weight2*data.col(index2) ) / (weight1+weight2) );
            
            // TODO : Implement QSLERP interpolation
        
        } else {
            
            if( ( pTime*mFrameRate ) > ( data.n_elem-1 ) ) {
                
                return arma::zeros( 1, 1 );
            
            } else return this->data.col( (int)(pTime*mFrameRate) );
        }
        
        return arma::zeros( 1, 1 );
    }
    
    bool TimedMat::set( const arma::vec &pData, unsigned int pIndex ) {
        
        if( mTimed ) return false;
        if( !checkInput(pData) ) return false;
        
        if( pIndex<data.n_elem ) {
            
            data.col( pIndex ) = pData;
            
            return true;
        
        } else {
            
            if( pIndex == data.n_elem ) {
                
                data.insert_cols( data.n_cols, 1 );
                data.col( data.n_cols-1 ) = pData;
                
                return true;
            
            } else return false;
        }
        
        return true;
    }
    
    bool TimedMat::set( const arma::vec &pData, double pTime ) {
        
        if( !checkInput(pData) ) return( false );
        
        if( mTimed ) {
            
            if( mTimeVec.n_elem == 0 ) return( false );
            
            if( mTimeVec(0) > pTime ) {
                
                data.insert_cols( 0, 1 );
                data.col(0) = pData;
                mTimeVec.insert_rows( 0, 1 );
                mTimeVec(0) = pTime;
                
                return true;
            }
            
            if( mTimeVec( mTimeVec.n_elem-1 ) < pTime ) {
                
                data.insert_cols( data.n_cols, 1 );
                data.col( data.n_cols-1 ) = pData;
                mTimeVec.insert_rows( mTimeVec.n_elem, 1 );
                mTimeVec( mTimeVec.n_elem-1 ) = pTime;
                
                return true;
            }
            
            arma::uvec index = find( this->mTimeVec == pTime );
            
            if( index.n_elem > 0 ) {
                
                data.col( index(0) ) = pData;
                
                return true;
            
            } else {
                
                index = find( this->mTimeVec > pTime );
                
                if( index.n_elem > 0 ) {
                    
                    data.insert_cols( index(0), 1 );
                    data.col( index(0) ) = pData;
                    mTimeVec.insert_rows( index(0), 1 );
                    mTimeVec(index(0)) = pTime;
                    
                    return true;
                
                } else return false;
            }
        
        } else {
            
            if( std::abs( pTime-mFrameRate*round(pTime/mFrameRate) ) <= 1E-6 ) {
                
                if( (int)round(pTime/mFrameRate) < data.n_cols ) {
                    
                    data.col( (int)round(pTime/mFrameRate) ) = pData;
                    
                    return true;
                
                } else return false;
            
            } else return false;
        }
    }
    
    bool TimedMat::push( const arma::vec &pData ) {
        
        if( mTimed ) return false; // First check if conditions to push
        if( data.n_elem > 0 && pData.n_rows != data.n_rows ) return false;
        
        data.insert_cols( data.n_cols, pData );
        
        return true;
    }
    
    bool TimedMat::push( const arma::vec &pData, double pTime ) {
        
        if( pTime<this->mTimeVec( mTimeVec.n_elem-1 ) ) return( false );
        this->set( pData, pTime );
        return( true );
        
    }
    
    void TimedMat::pop( void ) {
        
        data.shed_col( 0 ); // Shed one col/vec
        if( mTimed ) mTimeVec.shed_row( 0 );
    }
    
    TimedMat TimedMat::sub( int pBegIndex, int pEndIndex ) {
        
        TimedMat oneMat = *this;
        
        oneMat.data = data.submat( 0, pBegIndex, data.n_rows-1, pEndIndex ); // Chopping matrix
        if( isTimed() ) oneMat.getTimeVecRef() = getTimeVecRef().subvec( pBegIndex, pEndIndex );
        
        return( oneMat );
    }
    
    TimedVec TimedMat::elem( unsigned int pIndex ) {
    
        TimedVec elem;
        arma::vec elemVec;
        
        elemVec = data.row( pIndex ).t();
        
        if( mTimed ) elem.setData( mTimeVec, elemVec );
        else elem.setData( mFrameRate, elemVec );
        
        return( elem );
    }
    
    // == TIMED CUBE ==
    
    class TimedCube: public TimedData {
        
      public:
        
        TimedCube( void ) { interpolAlgo = LINEAR; }
        
        inline const arma::mat &get( unsigned int pIndex ); // Frame getter
        inline arma::mat get( double pTime ); // by index and time
        
        inline bool set( const arma::mat &pData, unsigned int pIndex ); // Frame setter
        inline bool set( const arma::mat &pData, double pTime ); // by index and time
        
        inline bool push( const arma::mat &pData, double pTime ); // Push timed
        inline bool push( const arma::mat &pData ); // Push at the end of data
        inline void pop( void ); // Pop oldest frame from the data container
        
        inline TimedCube sub( int pBegIndex, int pEndIndex ); // Chop by index
        inline TimedMat row( unsigned int pIndex ); // Row getter by index
        inline TimedMat col( unsigned int pIndex ); // Col getter by index
        
        bool setData( float pFrameRate, const arma::cube &pData ); // Set indexed
        bool setData( const arma::vec &pTime, const arma::cube &pData ); // Set timed
        
        inline const arma::cube &getData( void ) { return data; } // Get data copy
        inline arma::cube &getRefData( void ) { return data; } // Get data ref
        
        bool setInterpAlgo( InterpTypes interpAlgo ); // Set interp algo
        unsigned int nOfFrames( void ) { return( data.n_slices ); } // # of frames
        unsigned int nOfRows( void ) { return( data.n_rows ); } // # of rows
        unsigned int nOfCols( void ) { return( data.n_cols ); } // # of cols
        void clear( void ) { data.clear(); } // Clear
        
      protected:
        
        bool checkInput( const arma::mat &pData );
        
        InterpTypes interpolAlgo;
        arma::cube data;
    };
    
    // - Inlined functions -
    
    const arma::mat &TimedCube::get( unsigned int pIndex ) {
        
        return data.slice( pIndex );
    }
    
    arma::mat TimedCube::get( double pTime ) {
        
        if( mTimed ) {
            
            double weight1,weight2;
            unsigned int index1,index2;
            
            interpIndexFind( this->mTimeVec, pTime, index1, weight1, index2, weight2 );
            
            if( interpolAlgo == LINEAR ) {
                
                return( ( weight1 * data.slice( index1 ) + weight2
                         * data.slice( index2 ) )/ ( weight1+weight2 ) );
                
            } else if ( interpolAlgo == QSLERP ) {
                
                // TODO : Implement QSLERP interpolation
                return arma::zeros( 1, 1 );
                
            } else {
                
                return arma::zeros( 1, 1 );
            }
            
        } else {
            
            return this->data.slice( (int)( pTime*mFrameRate ) );
        }
        
        return arma::zeros( 1, 1 );
    }
    
    bool TimedCube::set( const arma::mat &pData, unsigned int pIndex ) {
        
        if( mTimed ) return false;
        if( !checkInput( pData ) ) return false;
        // -> No go check: not timed or bad format
        
        if( pIndex < data.n_elem ) {
            
            // Insert data into cube
            data.slice( pIndex ) = pData;
            return true;
            
        } else {
            
            if( pIndex == data.n_elem ) {
                
                // Insert data at the end of cube
                data.insert_slices( data.n_slices, 1 );
                data.slice( data.n_slices-1 ) = pData;
                return true;
                
            } else
                
                // Don't insert
                return false;
        }
    }
    
    bool TimedCube::set( const arma::mat &pData, const double pTime ) {
        
        if( !checkInput( pData ) ) return( false );
        
        if( mTimed ) {
            
            if( mTimeVec.n_elem == 0 ) return false;
            
            if( mTimeVec(0) > pTime ) {
                
                data.insert_slices( 0, 1 );
                data.slice(0) = pData;
                mTimeVec.insert_rows( 0, 1 );
                mTimeVec(0) = pTime;
                
                return true;
            }
            
            if( mTimeVec( mTimeVec.n_elem-1 ) < pTime ) {
                
                data.insert_slices( data.n_slices, 1 );
                data.slice( data.n_slices-1 ) = pData;
                mTimeVec.insert_rows( mTimeVec.n_elem, 1 );
                mTimeVec( mTimeVec.n_elem-1 ) = pTime;
                
                return true;
            }
            
            arma::uvec index = find( this->mTimeVec == pTime );
            
            if( index.n_elem > 0 ) {
                
                data.slice( index(0) ) = pData;
                
                return true;
            
            } else {
                
                index = find( this->mTimeVec > pTime );
                
                if( index.n_elem > 0 ) {
                    
                    data.insert_slices( index(0), 1 );
                    data.slice( index(0) ) = pData;
                    mTimeVec.insert_rows( index(0), 1 );
                    mTimeVec( index(0) ) = pTime;
                    
                    return true;
                
                } else return false;
            }
        
        } else {
            
            if( std::abs( pTime-this->mFrameRate*round( pTime/mFrameRate ) ) <= 1E-6 ) {
                
                if( (int)round( pTime/mFrameRate ) < data.n_slices ) {
                    
                    data.slice( (int)round( pTime/mFrameRate ) ) = pData;
                    
                    return true;
                
                } else return false;
            
            } else return false;
        }
    }
    
    bool TimedCube::push( const arma::mat &pData, double pTime ) {
        
        if( pTime < this->mTimeVec( mTimeVec.n_elem-1 ) ) return false;
        this->set( pData, pTime );
        
        return true;
    }
    
    bool TimedCube::push( const arma::mat &pData ) {
        
        if( data.n_elem > 0 && ( pData.n_rows != data.n_rows
        || pData.n_cols != data.n_cols ) ) return false;
        
        if( data.n_elem == 0 ) data.resize( pData.n_rows, pData.n_cols, 1 );
        else data.insert_slices( data.n_slices, 1 ); // Set the right slot
        
        data.slice( data.n_slices-1 ) = pData; // Copy data into slot
        
        return true;
    };
    
    void TimedCube::pop( void ) {
        
        if( mTimed ) mTimeVec.shed_row( 0 );
        data.shed_slice( 0 );
    }
    
    TimedCube TimedCube::sub( int pBegIndex, int pEndIndex ) {
        
        TimedCube oneCube = *this;
        
        oneCube.data = data.subcube( 0, 0, pBegIndex, data.n_rows-1, data.n_cols-1, pEndIndex );
        if( isTimed() ) oneCube.getTimeVecRef() = getTimeVecRef().subvec( pBegIndex, pEndIndex );
        
        return( oneCube );
    }
    
    TimedMat TimedCube::row( unsigned int pIndex ) {
        
        TimedMat trace;
        arma::mat traceMtx;
        
        traceMtx = data.tube( pIndex, 0, pIndex, data.n_cols-1 );
        
        if( mTimed ) trace.setData( mTimeVec, traceMtx );
        else trace.setData( mFrameRate, traceMtx );
        
        return( trace );
    }
    
    TimedMat TimedCube::col( unsigned int pIndex ) {
        
        TimedMat trace;
        arma::mat traceMtx;
        
        traceMtx = data.tube( 0, pIndex, data.n_rows-1, pIndex );
        
        if( mTimed ) trace.setData( mTimeVec, traceMtx );
        else trace.setData( mFrameRate, traceMtx );
        
        return( trace );
    }
};

#endif

/**
 *
 *  @file mmTrack.h
 *  @brief MotionMachine header file for Track class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmTrack__
#define __mmTrack__

#include <deque>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>

#include <armadillo>
#include "mmContainer.h"

#include "mmNodeList.h"
#include "mmBoneList.h"
#include "mmSynoList.h"

#include "mmFrame.h"
#include "mmTrace.h"
#include "mmNode.h"

#define MOMAINF 1000000000

namespace MoMa {
    
    class Track {
    
      public:
        
        Track( void ); // Default constructor
        virtual ~Track( void ); // Default destructor
        Track( Frame frame ); // Construt a track containing one frame

        void init( void ); // Initialize track

        // - Loading files -
        
        void load( std::string const &fileName ); // // Load & parse any file
        bool synolist( std::string fileName ); // Attach track to a given file of synonyms
        void nodes( std::string fileName ); // Attach track to a given node file
        void bones( std::string fileName ); // Attach track to a given bone file
        
        // - Extracting frames and frame-related matrices
        
        inline Frame frame( unsigned int index ); // Extract frame from track (by index)
        inline Frame frame( double time ); // Extract frame from track (by time tag)
        
        inline Frame operator[]( unsigned int index ); // Short version of frame()
        inline Frame operator[]( double time ); // Short version of frame()
        
        inline const arma::mat &framePosition( unsigned int index ); // Query frame by index in the track
        inline arma::mat framePosition( double time ); // Query frame by time in the track
        
        inline const arma::mat &frameRotation( unsigned int index ); // Query frame by index in the track
        inline arma::mat frameRotation( double time ); // Query frame by time in the track
        
        // - Extracting traces and trace-related matrices
        
        inline MoMa::Trace trace( std::string name ); // Extract trace (by name)
        inline MoMa::Trace trace( int index ); // Extract trace (by index)
        
        inline Trace operator()( std::string name ); // Short version of trace()
        inline Trace operator()( int index ); // Short version of trace()
        
        inline arma::mat tracePosition( int index ); // Query frame by index in the track
        inline arma::mat traceRotation( int index ); // Query frame by index in the track
        
        // - Ringbuffer-related methods -
        
        void setRingBufferSize( int size ,bool pHasRotation=false,bool pTimed=false); // Set as ring buffer + max size
        void subTrack( Track &subTr, int beg, int end ); // Extract a subtrack (into another track)
        void cut( int beg, int end); // Cut track's data from beg to end

        void copy(Track & tr);

        bool setJointOffsetRotation();
        
        void pushPosition( arma::mat frame ); // Add frame + checking if ringbuffer
     //   void popPosition( void ) { position.pop(); } // Remove frame

        void pushRotation( arma::mat frame ); // Add frame + checking if ringbuffer
      //  void popRotation( void ) { position.pop(); } // Remove frame

        void push( Frame _frame ); // Push new frame into the track (at the end)
        
        // - Misc -
        
        void setName( std::string name ); // Define track name
        void setFileName( std::string name ); // Define track name
        int index( std::string name ); // Get index from name
        
        void setFrameRate( float rate ); // Set/get frame rate
        inline float frameRate( void ) { return _frameRate; }
        
        inline double maxTime( void ); // Get the max time
        inline double minTime( void ); // Get the max time
        inline unsigned int nOfFrames( void ); // Get # frames
        inline unsigned int nOfNodes( void ); // Get # nodes
        void clear( void ); // Clear the track
        
        // protected:
        
        // TODO Re-protect this
        
        std::string easyName; // Track name
        std::string fileName; // Track file name
        
        TimedCube position; // Position frames
        TimedCube rotation; // Quaternion frames
        arma::mat rotationOffset; // Rotation offset
        bool hasRotation; // Has track rotations?
        bool hasOrigNodeRot_as_boneRot;//difference between V3D BVH skel and kinect
        
        NodeList *nodeList; // List of node names
        bool hasNodeList; // Has track nodeList?
        
        BoneList *boneList; // List of bone links
        bool hasBoneList; // Has track boneList?
        
        // SynoList *synoList; // List of synonyms?
        // bool hasSynoList; // Has track synoList?
        
        float _frameRate; // Track frame rate
        
        int ringSize; // Ringbuffer size
        bool isRing; // Is ringbuffer?
    };
    
    // - Inlined functions -
    
    Frame Track::frame( unsigned int index ) {
        
        Frame oneFrame;
        
        if( index < nOfFrames() ) {
            
            oneFrame.setRotationFlag( hasRotation );
            oneFrame.setPositionData( position.get( index ) );
            
            if( hasRotation ) { // We do copy rotation information only if necessary
                
                oneFrame.setRotationData( rotation.get( index ), rotationOffset );
            }
            
            oneFrame.hasNodeList = hasNodeList;
            oneFrame.nodeList = nodeList;
            
            oneFrame.hasBoneList = hasBoneList;
            oneFrame.boneList = boneList;
            
            // oneFrame.hasSynoList = hasSynoList;
            // oneFrame.synoList = synoList;
        }
        
        return( oneFrame );
    }
    
    Frame Track::frame( double time ) {
        
        Frame oneFrame;
        
        double maxTime = (double)nOfFrames() / (double)frameRate();
        
        //if( time >= 0.0f && time < maxTime ) 
		if (nOfFrames()>0)
		{
            
            oneFrame.setRotationFlag( hasRotation );
            oneFrame.setPositionData( position.get( time ) );
            
            if( hasRotation ) { // We do copy rotation information only if necessary
                
                oneFrame.setRotationData( rotation.get( time ), rotationOffset );
            }
            
            oneFrame.hasNodeList = hasNodeList;
            oneFrame.nodeList = nodeList;
            
            oneFrame.hasBoneList = hasBoneList;
            oneFrame.boneList = boneList;
            
            // oneFrame.hasSynoList = hasSynoList;
            // oneFrame.synoList = synoList;
        }
        
        return( oneFrame );
    }
    
    Frame Track::operator[]( unsigned int index ) {
        
        return( frame( index ) );
    }
    
    Frame Track::operator[]( double time ) {
        
        return( frame( time ) );
    }
    
    const arma::mat &Track::framePosition( unsigned int index ) {
    
        return( position.get( index ) );
    }
    
    arma::mat Track::framePosition( double time ) {
    
        return( position.get( time ) );
    }
    
    const arma::mat &Track::frameRotation( unsigned int index ) {
    
        return( rotation.get( index ) );
    }
    
    arma::mat Track::frameRotation( double time ) {
    
        return( rotation.get( time ) );
    }
    
    Trace Track::trace( std::string name ) {
        
        Trace oneTrace;
        int nIdx = -1; // Initialise
        
        if( hasNodeList ) {
        
            nIdx = nodeList->index( name );
            
            if( nIdx > -1 ) {
                
                return( this->trace( nIdx ) );
                
            } else {
            
                std::cout << "Track::trace(): Node not found" << std::endl;
                return( oneTrace );
            }
            
        } else {
        
            std::cout << "Track::trace(): No node name list" << std::endl;
            return( oneTrace );
        }
        
        /*
        bool isFound = false; // Flag
        
        if( hasNodeList ) {
            
            for( int n=0; n<nodeList->size(); n++ ) {
                
                // If we find a matching name, we save index
                if( nodeList->name(n).compare( name ) == 0 ) {
                    
                    isFound = true;
                    nIdx = n;
                }
            }
            
            if( hasSynoList && !isFound ) {
                
                for( int n=0; n<nodeList->size(); n++ ) {
                    
                    std::string nNameQuery = "nq"; // Synonym of name query
                    std::string nFromList = "fl"; // Synonym of nth in list
                    
                    synoList->search( name, nNameQuery ); // Search name
                    synoList->search( nodeList->name(n), nFromList ); // & list
                    
                    if( nFromList.compare( nNameQuery ) == 0 ) {
                        
                        nIdx = n;
                        isFound = true;
                    }
                }
            }
            
            if( isFound ) {
                
                // Return trace @ idx if found
                return( this->trace( nIdx ) );
                
            } else {
                
                std::cout << "Frame::node: Node not found" << std::endl;
            }
            
        } else {
            
            std::cout << "Frame::node: No node name list" << std::endl;
        }
        */
        
         // If not found, return empty trace
    }
    
    Trace Track::trace( int index ) {
        
        Trace oneTrace;
        
        if( nOfNodes() > 0 ) {
            
            oneTrace.setTimeFlag( true );
            oneTrace.setRotationFlag( hasRotation );
            oneTrace.setName( nodeList->name( index ) );

            if( position.isTimed() ) {
				if (position.isRealTime())
					oneTrace.position.setRealTimeMode(position.getBufferSize(),3);
                oneTrace.setPosition( position.getData().tube( 0, index, 2 , index ), position.getTimeVec() );
                
                
            } else {
                
				if (position.isRealTime())
					oneTrace.position.setRealTimeMode(position.getBufferSize(), position.frameRate(),3);
                oneTrace.setPosition( position.getData().tube( 0, index, 2, index ), position.frameRate(), position.initialTime() );
            }
            
            if( hasRotation ) {
                
                if( rotation.isTimed() ) {
                    
					if (rotation.isRealTime())
						oneTrace.rotation.setRealTimeMode(rotation.getBufferSize(),3);
                    oneTrace.setRotation( rotation.getData().tube( 0,index, 2, index ), position.getTimeVec() );
                    oneTrace.setRotationOffset( rotationOffset.col( index ) );
                    
                } else {
                    
					if (rotation.isRealTime())
						oneTrace.rotation.setRealTimeMode(rotation.getBufferSize(), rotation.frameRate(),3);
                    oneTrace.setRotation( rotation.getData().tube( 0, index, 2, index ), rotation.frameRate(), position.initialTime());
                    oneTrace.setRotationOffset( rotationOffset.col( index ) );
                }
            }          
        }
        
        return( oneTrace );
    }
    
    Trace Track::operator()( std::string name ){
        
        return( trace( name ) );
    }
    
    Trace Track::operator()( int index ) {
        
        return( trace( index ) );
    }
    
    arma::mat Track::tracePosition( int index ) {
        
        return( position.getData().tube( 0, index, 2 , index ) );
    }
    
    arma::mat Track::traceRotation( int index ) {
        
        return( rotation.getData().tube( 0, index, 2 , index ) );
    }
    
    double Track::maxTime( void ) {
        
        if( hasRotation ) return( std::max( position.maxTime(), rotation.maxTime() ) );
        else return( position.maxTime() ); // Make a robust version of this return
    }
    double Track::minTime( void ) {
        
        if( hasRotation ) return( std::min( position.minTime(), rotation.minTime() ) );
        else return( position.minTime() ); // Make a robust version of this return
    }
    
    unsigned int Track::nOfFrames( void ) {
        
        if( hasRotation ) return( std::max( position.nOfFrames(), rotation.nOfFrames() ) );
        else return( position.nOfFrames() ); // Make a robust version of this return
    }
    
    unsigned int Track::nOfNodes( void ) {
        
        if( hasRotation ) return( std::max( rotation.nOfCols(), position.nOfCols() ) );
        else return( position.nOfCols() ); // Make a robust version of this return
    }
}

#endif

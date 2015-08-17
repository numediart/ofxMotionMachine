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
        
        // - Loading files -
        
        void load( std::string const &fileName ); // // Load & parse any file
        void synolist( std::string fileName ); // Attach track to a given file of synonyms
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
        
        void setRingBufferSize( int size ); // Set as ring buffer + max size
        void subTrack( Track &subTr, int beg, int end ); // Extract a subtrack
        
        void pushPosition( arma::mat frame ); // Add frame + checking if ringbuffer
        void popPosition( void ) { position.pop(); } // Remove frame

        void pushRotation( arma::mat frame ); // Add frame + checking if ringbuffer
        void popRotation( void ) { position.pop(); } // Remove frame

        void push( Frame _frame ); // Push new frame into the track (at the end)
        
        // - Misc -
        
        void setName( std::string name ); // Define track name
        void setFileName( std::string name ); // Define track name
        int index( std::string name ); // Get index from name
        
        void setFrameRate( float rate ); // Set/get frame rate
        inline float frameRate( void ) { return _frameRate; }
        
        inline int nOfFrames( void ); // Get # frames
        inline int nOfNodes( void ); // Get # nodes
        void clear( void ); // Clear the track
        
        // unprotected:
        
        std::string easyName; // Track name
        std::string fileName; // Track file name
        
        TimedCube position; // Position frames
        TimedCube rotation; // Quaternion frames
        arma::mat rotationOffset; // Rotation offset
        bool hasRotation; // Has track rotations?
        
        NodeList *nodeList; // List of node names
        bool hasNodeList; // Has track nodeList?
        
        BoneList *boneList; // List of bone links
        bool hasBoneList; // Has track boneList?
        
        SynoList *synoList; // List of synonyms?
        bool hasSynoList; // Has track synoList?
        
        float _frameRate; // Track frame rate
        
        int ringSize; // Ringbuffer size
        bool isRing; // Is ringbuffer?
    };
    
    // - Inlined functions -
    
    Frame Track::frame( unsigned int index ) {
        
        Frame oneFrame;
        
        if( index < nOfFrames() ) {
            
            oneFrame.setRotationFlag( hasRotation );
            oneFrame.setPositionData( position.at( index ) );
            
            if( hasRotation ) { // We do copy rotation information only if necessary
                
                oneFrame.setRotationData( rotation.at( index ), rotationOffset );
            }
            
            oneFrame.hasNodeList = hasNodeList;
            oneFrame.nodeList = nodeList;
            
            oneFrame.hasBoneList = hasBoneList;
            oneFrame.boneList = boneList;
            
            oneFrame.hasSynoList = hasSynoList;
            oneFrame.synoList = synoList;
        }
        
        return( oneFrame );
    }
    
    Frame Track::frame( double time ) {
        
        Frame oneFrame;
        
        double maxTime = (double)nOfFrames() / (double)frameRate();
        
        if( time >= 0.0f && time < maxTime ) {
            
            oneFrame.setRotationFlag( hasRotation );
            oneFrame.setPositionData( position.at( time ) );
            
            if( hasRotation ) { // We do copy rotation information only if necessary
                
                oneFrame.setRotationData( rotation.at( time ), rotationOffset );
            }
            
            oneFrame.hasNodeList = hasNodeList;
            oneFrame.nodeList = nodeList;
            
            oneFrame.hasBoneList = hasBoneList;
            oneFrame.boneList = boneList;
            
            oneFrame.hasSynoList = hasSynoList;
            oneFrame.synoList = synoList;
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
    
        return( position.at( index ) );
    }
    
    arma::mat Track::framePosition( double time ) {
    
        return( position.at( time ) );
    }
    
    const arma::mat &Track::frameRotation( unsigned int index ) {
    
        return( rotation.at( index ) );
    }
    
    arma::mat Track::frameRotation( double time ) {
    
        return( rotation.at( time ) );
    }
    
    Trace Track::trace( std::string name ) {
        
        Trace oneTrace;
        int nIdx = -1; // Initialise
        bool isFound = false; // Flag
        
        if( hasNodeList ) {
            
            for( int n=0; n<nodeList->size(); n++ ) {
                
                // If we find a matching name, we save index
                if( nodeList->at(n).compare( name ) == 0 ) {
                    
                    isFound = true;
                    nIdx = n;
                }
            }
            
            if( hasSynoList && !isFound ) {
                
                for( int n=0; n<nodeList->size(); n++ ) {
                    
                    std::string nNameQuery = "nq"; // Synonym of name query
                    std::string nFromList = "fl"; // Synonym of nth in list
                    
                    synoList->search( name, nNameQuery ); // Search name
                    synoList->search( nodeList->at(n), nFromList ); // & list
                    
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
        
        return( oneTrace ); // If not found, return empty trace
    }
    
    Trace Track::trace( int index ) {
        
        Trace oneTrace;
        
        oneTrace.setTimeFlag( true );
        oneTrace.setRotationFlag( hasRotation );
        oneTrace.setName( nodeList->at( index ) );
        
        if( position.isTimed() ) {
            
            oneTrace.setPosition( position.getData().tube( 0, index, 2 , index ), position.getTimeVec() );
            
        } else {
            
            oneTrace.setPosition( position.getData().tube( 0,index, 2, index ), position.frameRate() );
        }
        
        if( hasRotation ) {
            
            if( rotation.isTimed() ) {
                
                oneTrace.setRotation( rotation.getData().tube( 0,index, 2, index ), position.getTimeVec() );
                oneTrace.setRotationOffset( rotationOffset.col( index ) );
                
            } else {
                
                oneTrace.setRotation( rotation.getData().tube( 0, index, 2, index ), position.frameRate() );
                oneTrace.setRotationOffset( rotationOffset.col( index ) );
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
    
    int Track::nOfFrames( void ) {
        
        return( std::max( position.nOfFrames(), rotation.nOfFrames() ) );
    }
    
    int Track::nOfNodes( void ) {
        
        return( std::max( rotation.nOfCols(), position.nOfCols() ) );
    }
}

#endif

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

namespace MoMa {
    
    enum FileType {
    
        FLAT,
        V3D,
        C3D,
        BVH
    };
    
    class Track {
    
      public:
        
        Track( void ); // Default constructor
        virtual ~Track( void ); // Default destructor
        
        void nodes( std::string fileName ); // Attach track to a given node file
        void bones( std::string fileName ); // Attach track to a given bone file
        void load( std::string const &fileName ); // // Load & parse any file
        
        inline arma::mat positions( int index ); // Query frame by index in the track
        arma::mat positions( double time ); // Query frame by time in the track
        
        inline arma::mat rotations( int index ); // Query frame by index in the track
        arma::mat rotations( double time ); // Query frame by time in the track
        
        inline Frame frame( int index ); // Extract frame from track (by index)
        inline Frame frame( double time ); // Extract frame from track (by time tag)
        
        inline Frame operator[]( int index ); // Short version of frame()
        inline Frame operator[]( double time ); // Short version of frame()
        
        void setRingBufferSize( int size ); // Set as ring buffer + max size
        void subTrack( Track &subTr, int beg, int end ); // Extract a subtrack
        
        void pushPosition( arma::mat frame ); // Add frame + checking if ringbuffer
        void popPosition( void ) { position.pop_front(); } // Remove frame

        void pushRotations( arma::mat frame ); // Add frame + checking if ringbuffer
        void popRotations( void ) { position.pop_front(); } // Remove frame

        void push( Frame _frame ); // Push new frame into the track (at the end)
        
        MoMa::Trace trace( std::string name ); // Extract trace (by name)
        MoMa::Trace trace( int index ); // Extract trace (by index)
        
        inline Trace operator()( std::string name ); // Short version of trace()
        inline Trace operator()( int index ); // Short version of trace()
        
        void setName( std::string name ); // Define track name
        void setFileName( std::string name ); // Define track name
        int index( std::string name ); // Get index from name
        
        inline int nOfFrames( void ); // Get # frames
        inline int nOfNodes( void ); // Get # nodes
        void clear( void ); // Clear the track
        
        // ---
        
        std::string easyName; // Track name
        std::string fileName; // Track file name
        
        timed3dContainer position; // Position frames
        timed3dContainer rotation; // Quaternion frames
        arma::mat rotationOffset; // Rotation offset
        bool hasRotation; // Has track rotations?
        
        NodeList *nodeList; // List of node names
        bool hasNodeList; // Has track nodeList?
        
        BoneList *boneList; // List of bone links
        bool hasBoneList; // Has track boneList?
        
        SynoList *synoList; // List of synonyms?
        bool hasSynoList; // Has track synoList?
        
        float frameRate; // Track frame rate
        
        int ringSize; // Ringbuffer size
        bool isRing; // Is ringbuffer?
    };
    
    // Inlined functions
    
    Frame Track::frame( int index ) {
        
        Frame oneFrame;
        
        if( index >= 0 && index < nOfFrames() ) {
            
            oneFrame.setPositionData(position.getIndexedFrame(index));
            
            oneFrame.setRotationFlag( hasRotation );
            
            if( hasRotation ){
                
                oneFrame.setRotationData( rotation.getIndexedFrame(index), rotationOffset );
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
        
        oneFrame.setPositionData( position.getTimedFrame( time ) );
        oneFrame.setRotationFlag( hasRotation );
        
        if( hasRotation ) {
            
            oneFrame.setRotationData( rotation.getTimedFrame( time ), rotationOffset );
        }
        
        oneFrame.hasNodeList = hasNodeList;
        oneFrame.nodeList = nodeList;
        
        oneFrame.hasBoneList = hasBoneList;
        oneFrame.boneList = boneList;
        
        oneFrame.hasSynoList = hasSynoList;
        oneFrame.synoList = synoList;
        
        return( oneFrame );
    }
    
    int Track::nOfFrames( void ) {
        
        return( std::max( position.nOfFrames(), rotation.nOfFrames() ) );
    }
    
    int Track::nOfNodes( void ) {
        
        return( std::max( rotation.nOfNodes(), position.nOfNodes() ) );
    }
    
    Frame Track::operator[]( int index ) {
    
        return( frame( index ) );
    }
    
    Frame Track::operator[]( double time ) {
    
        return( frame( time ) );
    }
    
    Trace Track::operator()( std::string name ){
    
        return( trace( name ) );
    }
    
    Trace Track::operator()( int index ) {
    
        return( trace( index ) );
    }
}

#endif

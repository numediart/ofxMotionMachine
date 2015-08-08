/**
 *
 *  @file mmTrace.h
 *  @brief MotionMachine header file for Trace class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmTrace__
#define __mmTrace__

#include <vector>
#include <iostream>

#include <armadillo>
#include "mmContainer.h"

#include "mmNode.h"

namespace MoMa {

    class Trace {
        
        // TODO It seems the new traces are "transposed"
        // compared to what is expected. It gives me the
        // nOfFrames() = 3 and lots of dims vs. lots of
        // frames and 3 dims, i.e. x,y,z of the node.
        
      public:
        
        Trace( void ); // Default constructor
        
        std::string name( void ) { return( _name ); } // Get name
        void setName( std::string n ) { _name = n; } // Set name
        
        bool hasTime( void ) { return( position.isTimestamped() &&(hasRotation()?rotation.isTimestamped():1)); } // Flag
        void setTimeFlag( bool tim ) { _hasTime = tim; } // Force it//TODO verify if it is necessary, a trace has necessarely a time, but not a timedstamp time scale.
        
        bool hasRotation( void ) { return( _hasRotation ); } // Use rotation?
        inline void setRotationFlag( bool rot ) { _hasRotation = rot; } // Force it
        arma::mat matrix( void ) ;
        Node nodeFrame( double time );
        inline arma::vec  getFramePosition( int index ) { return( position.getIndexedFrame(index) ); } // By index
        arma::vec getFramePosition( double time ){ return( position.getTimedFrame(time) ); }; // Query node by time in the trace
        inline arma::vec  getFramerotation( int index ) { return( rotation.getIndexedFrame(index) ); } // By index
        arma::vec getFramerotation( double time ){ return( rotation.getTimedFrame(time) ); }; // Query node by time in the trace
        inline arma::vec  nodeRotationOffset( ) { return( rotationOffset ); } // By index
        
        
        void setPosition(const arma::mat &data,float fr){position.setData(fr, data);};
        void setRotation(const arma::mat &data,float fr){rotation.setData(fr, data);};
        void setRotationOffset(const arma::vec &data){rotationOffset = data;};
        void setPosition(const arma::mat &data,const arma::vec &time){position.setTimedData(time, data);};
        void setRotation(const arma::mat &data,const arma::vec &time ){rotation.setTimedData(time, data);};
        
        inline arma::vec  operator[]( int index ); // Short version of node()
        inline arma::vec  operator[]( double time ); // Short version of node()
        
        inline void push( Node node ) {
            if (!position.isTimestamped()){
                position.pushFrame( node.position);
                if (_hasRotation&&node.hasRotation()){
                    rotation.pushFrame(node.rotation);
                    if (rotationOffset.n_elem==0)
                        rotationOffset=node.rotationOffset;
                }
            }
            else{
                position.pushTimedFrame( node.position,node.time());
                if (_hasRotation&&node.hasRotation()){
                    rotation.pushTimedFrame(node.rotation,node.time());
                    if (rotationOffset.n_elem==0)
                        rotationOffset=node.rotationOffset;
                }
            }
        
        } // Add node
        
        inline arma::mat getRotation( void ) {return rotation.getData();}; // Extract Armadillo matrix from trace
        inline arma::mat getPosition( void ) {return position.getData();}; // Extract Armadillo matrix from trace
        arma::mat m( void ){return _hasRotation?arma::join_rows(position.getData(),rotation.getData()):position.getData();}; // same thing shorter
        // TODO void setMatrix( mat data ); // Set trace from matrix
        
        inline int nOfFrames( void ){ // Get number of frmae in the trace
            return( std::max(position.nOfFrames(),rotation.nOfFrames()));
        }
        void print( void ); // Print the content of the trace
        
    protected:
        timed2dContainer position;
        timed2dContainer rotation;
        arma::vec rotationOffset;
        bool _hasTime;
        bool _hasRotation;
        std::string _name;
    };
    
    // Inlined functions
    
    arma::vec Trace::operator[]( int index ){
        
        return( position.getIndexedFrame(index) );
    }
    
    arma::vec Trace::operator[]( double time ){
        
        return( position.getTimedFrame(time ) );
    }
}

#endif

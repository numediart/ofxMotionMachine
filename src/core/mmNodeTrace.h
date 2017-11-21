/**
 *
 *  @file mmNodeTrace.h
 *  @brief MotionMachine header file for NodeTrace class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmNodeTrace__
#define __mmNodeTrace__

#include <vector>
#include <iostream>

#include <armadillo>
#include "mmContainer.h"

#include "mmNode.h"

namespace MoMa {

    class NodeTrace {
        
        // TODO It seems the new traces are "transposed"
        // compared to what is expected. It gives me the
        // nOfFrames() = 3 and lots of dims vs. lots of
        // frames and 3 dims, i.e. x,y,z of the node.
        
      public:
        
        NodeTrace( void ); // Default constructor
        
        std::string name( void ) const{ return( _name ); } // Get name
        inline void setName( std::string n ) { _name = n; } // Set name
        
        bool hasTime( void ) const { return( position.isTimed() ); } // Flag
        inline void setTimeFlag( bool tim ) { _hasTime = tim; } // Force it//TODO verify if it is necessary, a trace has necessarely a time, but not a timedstamp time scale.
        
        arma::mat matrix( void ) ;
        inline arma::vec getFramePosition( unsigned int index ) { return( position.get(index) ); } // By index
        arma::vec getFramePosition( double time ){ return( position.get(time) ); }; // Query node by time in the trace
        
        inline void setPosition(const arma::mat &data,float fr, float initT = 0.0f){position.setData(fr, data, initT);};
        inline void setPosition(const arma::mat &data,const arma::vec &time){position.setData(time, data);};
        
        inline Node node(unsigned int index);
        inline Node node(double time);

        inline Node operator[]( unsigned int index ); // Short version of node()
        inline Node operator[]( double time ); // Short version of node()
        
        inline void push( Node node ) {
            if (!position.isTimed()){
                position.push( node.position );
            }
            else{
                position.push( node.position,node.time() );
            }
        
        } // Add node
        
        inline arma::mat getPosition( void ) {return position.getData();}; // Extract Armadillo matrix from trace
        arma::mat m( void ){return position.getData();}; // same thing shorter
        // TODO void setMatrix( mat data ); // Set trace from matrix
        
        inline int nOfFrames( void ){ // Get number of frmae in the trace
            return( position.nOfFrames() );
        }
        void print( void ); // Print the content of the trace
        
    // protected:
        
        // TODO Re-protect this
        
        TimedMat position;
        bool _hasTime;
        std::string _name;
    };
    
    // Inlined functions
    
    Node NodeTrace::node(unsigned int index) {

        return(Node(position.get(index), _name));
    }

    Node NodeTrace::node(double time) {

        return(Node(position.get(time), _name, true, time));
    }

    Node NodeTrace::operator[]( unsigned int index ){
        
        return( Node(position.get(index), _name) );
    }
    
    Node NodeTrace::operator[]( double time ){
        
        return(Node(position.get(time), _name, true, time));
    }

    typedef NodeTrace Trace;
}

#endif

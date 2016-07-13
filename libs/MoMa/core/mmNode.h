/**
 *
 *  @file mmNode.h
 *  @brief MotionMachine header file for Node class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 * 
 */

#ifndef __mmNode__
#define __mmNode__

#include <cstring>
#include <armadillo>

namespace MoMa {

#ifndef _PositionDimension_
#define _PositionDimension_
	enum PositionDimension {

		X, // Position or rotation @ x
		Y, // Position or rotation @ y
		Z, // Position or rotation @ z
		T // Index for the time-stamp
	};
#endif
  /*  
    enum RotationDimension {
        
        qX, // Quaternion @ x
        qY, // Quaternion @ y
        qZ, // Quaternion @ z
        qW // Quaternion @ w
    };
    */
    class Node {
        
      public:
        
        Node( std::string name = "", bool hasTime = false, float time = 0.0f ); // Default constructor
        Node( float x, float y, float z, std::string name = "", bool hasTime = false, float time = 0.0f); // Other kinds of constructors
        Node(arma::vec pos, std::string name = "", bool hasTime = false, float time = 0.0f);
    //    Node( float x, float y, float z, float qx, float qy, float qz, float qw );
      //  Node( arma::vec coord, arma::vec rotation, arma::vec offsetRot);
        
        void setPosition( float x, float y, float z ); // Set position
      //  void setRotation( float x, float y, float z , float w); // Set rotation
      //  void setOffsetRotation( float x, float y, float z , float w); // Set offset
        
        void setPosition( arma::vec pos); // Set position
        //void setRotation( arma::vec rot); // Set rotation
        //void setOffsetRotation( arma::vec off); // Set offset
        
        //inline bool hasRotation( void ) const { return( _hasRotation ); } // Use rotation?
        //inline void setRotationFlag( bool rot ) { _hasRotation = rot; } // Force it
        
        double time( void )const { return( _time ); } // Get time
        bool hasTime( void )const { return( _hasTime ); } // Flag
        inline void setTime( double t ); // Set time tag
        
        std::string name( void ) const{ return( _name ); } // Get name
        inline void setName( std::string n ) { _name = n; } // Set name
        
        int state( void ) const { return( _state ); }
        void setState( int st ) { _state = st; }
        
        arma::vec position; // 3D position of the node
        //arma::vec rotation; // Quaternion rotation
        //arma::vec rotationOffset; // Quaternion offset
        
      protected:
        
        //bool _hasRotation; // Do we use rotation?
        double _time; // Transfered time stamp
        bool _hasTime; // Any time stamp?
        std::string _name; // Node name
        int _state; // Node state
    };
    
    // Inlined functions
    
    void Node::setTime( double t ) {
        
        _hasTime = true;
        _time = t;
    }
}

#endif

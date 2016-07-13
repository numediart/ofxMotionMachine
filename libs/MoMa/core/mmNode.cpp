/**
 *
 *  @file mmNode.cpp
 *  @brief MotionMachine source file for Node class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmNode.h"

using namespace std;
using namespace arma;

MoMa::Node::Node( string name, bool hasTime, float time) {
    
    position.resize( 3 );
//    rotation.resize( 4 );
//   rotationOffset.resize( 4 );
    
    position( X ) = datum::nan;
    position( Y ) = datum::nan;
    position( Z ) = datum::nan;
    
    //rotation( X ) = 0.0f;
    //rotation( Y ) = 0.0f;
    //rotation( Z ) = 0.0f;
    //rotation( 3 ) = 1.0f;
    //
    //rotationOffset( X ) = 0.0f;
    //rotationOffset( Y ) = 0.0f;
    //rotationOffset( Z ) = 0.0f;
    //rotationOffset( 3 ) = 1.0f;
    
    //_hasRotation = false;
    _hasTime = hasTime;
    _time = time;
    _name = name;
}

MoMa::Node::Node( float x, float y, float z, string name, bool hasTime, float time) {
    
    position.resize( 3 );
    //rotation.resize( 4 );
    //rotationOffset.resize( 4 );
    
    position( X ) = x;
    position( Y ) = y;
    position( Z ) = z;
    
    //rotation( X ) = 0.0f;
    //rotation( Y ) = 0.0f;
    //rotation( Z ) = 0.0f;
    //rotation( 3 ) = 1.0f;
    
    //rotationOffset( X ) = 0.0f;
    //rotationOffset( Y ) = 0.0f;
    //rotationOffset( Z ) = 0.0f;
    //rotationOffset( 3 ) = 1.0f;
    
    //_hasRotation = false;
    _hasTime = hasTime;
    _time = time;
    _name = name;
    
}

MoMa::Node::Node(vec pos, string name, bool hasTime, float time) {
        
    setPosition(pos);
    position.resize(3);
    _hasTime = hasTime;
    _time = time;
    _name = name;
}

/*MoMa::Node::Node( float x, float y, float z, float qx, float qy, float qz, float qw ) {
    
    position.resize( 3 );
    rotation.resize( 4 );
    rotationOffset.resize( 4 );
    
    position( X ) = x;
    position( Y ) = y;
    position( Z ) = z;
    
    rotation( X ) = qx;
    rotation( Y ) = qy;
    rotation( Z ) = qz;
    rotation( 3 ) = qw;
    
    rotationOffset( X ) = 0.0f;
    rotationOffset( Y ) = 0.0f;
    rotationOffset( Z ) = 0.0f;
    rotationOffset( 3 ) = 1.0f;
    
    _hasRotation = true;
    _hasTime = false;
    _time = 0.0f;
    _name = "";
    
}*/
/*MoMa::Node::Node( arma::vec coord, arma::vec rot, arma::vec offsetRot) {
    
    position.resize( 3 );
    rotation.resize( 4 );
    rotationOffset.resize( 4 );
    
    position( X ) = coord(0);
    position( Y ) = coord(1);
    position( Z ) = coord(2);
    
    rotation( X ) = rot(0);
    rotation( Y ) = rot(1);
    rotation( Z ) = rot(2);
    rotation( 3 ) = rot(3);
    
    rotationOffset( X ) = offsetRot(0);
    rotationOffset( Y ) = offsetRot(1);
    rotationOffset( Z ) = offsetRot(2);
    rotationOffset( 3 ) = offsetRot(3);
    
    _hasRotation = true;
    _hasTime = false;
    _time = 0.0f;
    _name = "";
    
}*/

void MoMa::Node::setPosition( float x, float y, float z ) {
    
    position( X ) = x;
    position( Y ) = y;
    position( Z ) = z;
}

/*void MoMa::Node::setRotation( float x, float y, float z, float w ) {
    
    _hasRotation = true;
    
    if( x*x+y*y+z*z+w*w != 0 ) {
        
        rotation( qX ) = x;
        rotation( qY ) = y;
        rotation( qZ ) = z;
        rotation( qW ) = w;
    }
}*/
/*void MoMa::Node::setOffsetRotation( float x, float y, float z, float w ) {
    
    _hasRotation = true;
    
    if( x*x+y*y+z*z+w*w != 0 ) {
        
        rotationOffset( qX ) = x;
        rotationOffset( qY ) = y;
        rotationOffset( qZ ) = z;
        rotationOffset( qW ) = w;
    }
}*/

void MoMa::Node::setPosition( arma::vec pos ) {
    
    position = pos;
}

/*void MoMa::Node::setRotation( arma::vec rot ) {
    
    if( arma::norm(rot) != 0 ) {
        
        _hasRotation = true;
        rotation=rot;
    }
}
void MoMa::Node::setOffsetRotation( arma::vec off ) {
    
    if( arma::norm(off)!=0 ) {
        
        _hasRotation = true;
        rotationOffset=off;
    }
}*/

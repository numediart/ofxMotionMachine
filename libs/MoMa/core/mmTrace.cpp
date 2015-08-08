/**
 *
 *  @file mmTrace.cpp
 *  @brief MotionMachine source file for Trace class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmTrace.h"

using namespace std;
using namespace arma;
using namespace MoMa;

Trace::Trace( void ) {

    _hasRotation = false;
    _hasTime = false;
    _name = "";
}

Node Trace::nodeFrame( double time ) {

    Node oneNode;
    oneNode.setPosition(position.getTimedFrame(time));
    oneNode.setName(this->name());
    oneNode.setRotationFlag( hasRotation() );
    if (hasRotation()){
        oneNode.setRotation(rotation.getTimedFrame(time));
        oneNode.setOffsetRotation(rotationOffset);
    }
    if (hasTime())
        oneNode.setTime( time);
    return( oneNode );
}

mat Trace::matrix( void ) {
 
    mat trace;
    // int nOfDims = 3;

    trace=position.getData();
    if( hasRotation() ) trace=arma::join_cols(trace, rotation.getData());
    if( position.isTimestamped()&&hasTime() ) trace=arma::join_cols(trace,position.getTimeStamps())  ;
    
    
    return( trace );
}



void Trace::print( void ) {

    cout << "--- # of frames: " << nOfFrames();
    cout << " ---" << endl;
    {
        
        cout << position.getData()<< "\n";
        if( hasRotation()  ) {
            cout << rotation.getData()<< "\n";
        }
        if( hasTime() || position.isTimestamped() ) {
            cout<<position.getTimeStamps();
        }
        
        cout << endl;
    }
}

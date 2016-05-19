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
    oneNode.setPosition(position.get(time));
    oneNode.setName(this->name());
    //oneNode.setRotationFlag( hasRotation() );
    //if (hasRotation()){
    //    oneNode.setRotation(rotation.get(time));
    //    oneNode.setOffsetRotation(rotationOffset);
    //}
    if (hasTime())
        oneNode.setTime( time);
    return( oneNode );
}

mat Trace::matrix( void ) {
 
    mat trace;
    // int nOfDims = 3;

    trace=position.getData();
    if( hasRotation() ) trace=arma::join_cols(trace, rotation.getData());
    if( position.isTimed()&&hasTime() ) trace=arma::join_cols(trace,position.getTimeVec())  ;
    
    
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
        if( hasTime() || position.isTimed() ) {
            cout<<position.getTimeVec();
        }
        
        cout << endl;
    }
}

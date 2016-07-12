/**
 *
 *  @file mmBoneTrace.cpp
 *  @brief MotionMachine source file for BoneTrace class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmBoneTrace.h"

using namespace std;
using namespace arma;
using namespace MoMa;

BoneTrace::BoneTrace( void ) {

    _hasRotation = false;
    _hasTime = false;
    _name = "";
}

mat BoneTrace::matrix( void ) {
 
    mat trace;
    // int nOfDims = 3;

    trace=position.getData();
    if( hasRotation() ) trace=arma::join_cols(trace, rotation.getData());
    if( position.isTimed()&&hasTime() ) trace=arma::join_cols(trace,position.getTimeVec())  ;
    
    
    return( trace );
}



void BoneTrace::print( void ) {

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

/**
 *
 *  @file mmNodeTrace.cpp
 *  @brief MotionMachine source file for NodeTrace class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmNodeTrace.h"

using namespace std;
using namespace arma;
using namespace MoMa;

NodeTrace::NodeTrace( void ) {

    _hasTime = false;
    _name = "";
}

mat NodeTrace::matrix( void ) {
 
    mat trace;
    // int nOfDims = 3;

    trace=position.getData();
    if( position.isTimed()&&hasTime() ) trace=arma::join_cols(trace,position.getTimeVec());    
    
    return( trace );
}

void NodeTrace::print( void ) {

    cout << "--- # of frames: " << nOfFrames();
    cout << " ---" << endl;
    {
        
        cout << position.getData()<< "\n";
        
        if( hasTime() || position.isTimed() ) {
            cout<<position.getTimeVec();
        }
        
        cout << endl;
    }
}

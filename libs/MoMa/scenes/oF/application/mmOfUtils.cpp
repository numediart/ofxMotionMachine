//
//  mmOfUtils.cpp
//  MotionMachine
//
//  Created by Thierry Ravet on 2/04/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#include "mmOfUtils.h"

using namespace arma;
namespace MoMa {
    ofVec3f toVec3f( vec data ) {
        
        // Create a oF 3D vector from a 3-dim Armadillo vector
        return( ofVec3f( data( 0 ), data( 1 ), data( 2 ) ) );
    }
    
    ofQuaternion toQuaternion( vec data ) {
        
        // Create a oF Quaternion from a 4-dim Armadillo vector
        return( ofQuaternion( data( 0 ), data( 1 ), data( 2 ),data(3) ) );
    }
}
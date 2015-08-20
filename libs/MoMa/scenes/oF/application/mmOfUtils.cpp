//
//  mmOfUtils.cpp
//  MotionMachine
//
//  Created by Thierry Ravet on 2/04/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#include <moma/mmTrack.h>

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
    
    ofVec3f getNodePosition( std::string bonename, MoMa::Track track, int frame ) {
        unsigned int bID = track.index( bonename );
        if ( bID == -1 ) {
            std::cerr << "MoMa::getBonePosition: '" << bonename << "' is not defined int this track!!!" << endl;
            return ofVec3f();
        }
        return toVec3f( track.position.getData().slice( frame ).col( bID ) );
    }
    
    
    ofQuaternion getNodeRotation( std::string bonename, MoMa::Track track, int frame ) {
        unsigned int bID = track.index( bonename );
        if ( bID == -1 ) {
            std::cerr << "MoMa::getBoneRotation: '" << bonename << "' is not defined int this track!!!" << endl;
            return ofVec3f();
        }
        return toQuaternion( track.rotation.getData().slice( frame ).col( bID ) );
    }
    
}
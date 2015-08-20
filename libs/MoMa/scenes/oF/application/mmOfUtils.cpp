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
    
    ofVec3f getNodePosition( std::string nodename, MoMa::Track &track, int frame ) {
        unsigned int nID = track.index( nodename );
        if ( nID == -1 ) {
            std::cerr << "MoMa::getBonePosition: '" << nodename << "' is not defined int this track!!!" << endl;
            return ofVec3f();
        }
        return toVec3f( track.position.getData().slice( frame ).col( nID ) );
    }
    
    
    ofQuaternion getNodeRotation( std::string nodename, MoMa::Track &track, int frame ) {
        unsigned int nID = track.index( nodename );
        if ( nID == -1 ) {
            std::cerr << "MoMa::getNodeRotation: '" << nodename << "' is not defined int this track!!!" << endl;
            return ofQuaternion();
        }
        return toQuaternion( track.rotation.getData().slice( frame ).col( nID ) );
    }
    
    MoMaBone getBoneByTail( std::string tailname, Track &track, int frame ) {
        unsigned int nID = track.index( tailname );
        if ( nID == -1 ) {
            std::cerr << "MoMa::getBoneByTail: '" << tailname << "' is not defined int this track!!!" << endl;
            MoMaBone mmb;
            mmb.exists = false;
            return mmb;
        }
        vector< std::pair<int,int> > * bones = ( vector< std::pair<int,int> > * ) track.boneList;
        for ( vector< std::pair<int,int> >::iterator itb = bones->begin(); itb != bones->end(); itb++ ) {
            if ( (*itb).second == nID ) {
                MoMaBone mmb;
                mmb.exists = true;
                mmb.head = toVec3f( track.position.getData().slice( frame ).col( (*itb).first ) );
                mmb.tail = toVec3f( track.position.getData().slice( frame ).col( nID ) );
                return mmb;
            }
        }
        MoMaBone mmb;
        mmb.exists = false;
        return mmb;
    }
    
}
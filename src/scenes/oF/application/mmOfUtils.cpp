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
    
    ofVec3f getNodePosition( std::string nodename, MoMa::Track &track, int frame ) {
        int nID = getNodeID( nodename, track );
        if ( nID == -1 ) return ofVec3f();
        return toVec3f( track.position.getData().slice( frame ).col( nID ) );
    }
    
    
    ofQuaternion getNodeRotation( std::string nodename, MoMa::Track &track, int frame ) {
        int nID = getNodeID( nodename, track );
        if ( nID == -1 ) return ofQuaternion();
        return toQuaternion( track.rotation.getData().slice( frame ).col( nID ) );
    }
    
    int getNodeID( std::string nodename, Track &track ) {
        int nID = track.index( nodename );
        if ( nID == -1 ) {
            std::cerr << "MoMa::getBoneID: '" << nodename << "' is not defined int this track!!!" << endl;
        }
        return nID;
    }
    
    int getNodeOriginID( std::string nodename, Track &track ) {
        int nID = track.index( nodename );
        if ( nID == -1 ) {
            std::cerr << "MoMa::getBoneID: '" << nodename << "' is not defined int this track!!!" << endl;
        } else {
            vector< std::pair<int,int> > * bones = ( vector< std::pair<int,int> > * ) track.boneList.get();
            for ( vector< std::pair<int,int> >::iterator itb = bones->begin(); itb != bones->end(); itb++ ) {
                if ( (*itb).second == nID ) return (*itb).first;
            }
        }
        return -1;
    }
    
    vector< unsigned int > getNoneChildrensID( std::string nodename, Track &track ) {
        vector< unsigned int > out;
        int nID = getNodeID( nodename, track );
        if ( nID != -1 ) {
            vector< std::pair<int,int> > * bones = ( vector< std::pair<int,int> > * ) track.boneList.get();
            for ( vector< std::pair<int,int> >::iterator itb = bones->begin(); itb != bones->end(); itb++ ) {
                if ( (*itb).first == nID ) out.push_back( (*itb).second );
            }
        }
        return out;
    }
    
}
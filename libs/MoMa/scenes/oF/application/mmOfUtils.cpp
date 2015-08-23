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
    
    /*** FEATURES ***/
    
    MoMaFeature::MoMaFeature( string name, Track * track, arma::mat * src ) {
        
        this->name = name;
        this->track = track;
        initialised = false;
        
// hardcoded and thus trashy!
        
        if ( name == "solicitation" ) {
            
            // 14 recognised nodes for solicitation
            rows = 14;
            cols = track->nOfFrames();
            string st[]={
                "Neck","LShoulder","RShoulder","LElbow",
                "RElbow","LWrist","RWrist","Pelvis",
                "LHip","RHip","LKnee","RKnee",
                "LAnkle","RAnkle"};
            vector<string> * nodes = track->nodeList;
            int nid = 0;
            for ( vector< string >::iterator itn = nodes->begin(); itn != nodes->end(); itn++ ) {
                ids_map[ nid ] = -1;
                for ( int i = 0; i < rows; i++ ) {
                    if ( st[ i ] == (*itn) ) {
                        ids_map[ nid ] = i;
                        break;
                    }
                }
                nid++;
            }
            process( src );
            initialised = true;
            return;

        } else {
            
            cout << "Unrecognised feature name! Use:" << endl
                    << "\t* 'solicitation'" << endl;
            return;
            
        }
        
    }
    
    MoMaFeature::~MoMaFeature() {}
    
    void MoMaFeature::process( arma::mat * src ) {
        raw = mat( rows, cols );
        normalised_local = mat( rows, cols );
        normalised_global = mat( rows, cols );
        delta = mat( rows, cols );
        arma::vec local_min( rows );
        arma::vec local_max( rows );
        float global_min = 0;
        float global_max = 0;
        unsigned int i = 0;
        for ( int r = 0; r < rows; r++ ) {
        for ( int c = 0; c < cols; c++ ) {
            float v = (*src)( r,c );
            raw( r,c ) = v;
            if ( c == 0 ) delta( r,c ) = 0;
            else delta( r,c ) = v - raw( r, c - 1 );
            if ( i == 0 || v < local_min(r) ) local_min(r) = v;
            if ( i == 0 || v > local_max(r) ) local_max(r) = v;
            if ( i == 0 || v < global_min ) global_min = v;
            if ( i == 0 || v > global_max ) global_max = v;
            i++;
        }
        }
        float global_div = global_max - global_min;
        for ( int r = 0; r < rows; r++ ) {
        for ( int c = 0; c < cols; c++ ) {
            normalised_local( r,c ) = ( raw(r,c) - local_min(r) ) / ( local_max(r) - local_min(r) );
            normalised_global( r,c ) = ( raw(r,c) - global_min ) / global_div;
        }
        }
    }
    
    bool MoMaFeature::hasValue( unsigned int nodeID ) {
        if ( ids_map[ nodeID ] == -1 ) return false;
        return true;
    }
    
    float MoMaFeature::getRaw( unsigned int nodeID, unsigned int frame ) {
        if ( ids_map[ nodeID ] == -1 ) return -1;
        return raw( ids_map[ nodeID ], frame );
    }

    float MoMaFeature::getNormalisedLocal( unsigned int nodeID, unsigned int frame ) {
        if ( ids_map[ nodeID ] == -1 ) return -1;
        return normalised_local( ids_map[ nodeID ], frame );
    }
    
    float MoMaFeature::getNormalisedGlobal( unsigned int nodeID, unsigned int frame ) {
        if ( ids_map[ nodeID ] == -1 ) return -1;
        return normalised_global( ids_map[ nodeID ], frame );
    }
    
    float MoMaFeature::getDelta( unsigned int nodeID, unsigned int frame ) {
        if ( ids_map[ nodeID ] == -1 ) return -1;
        return delta( ids_map[ nodeID ], frame );
    }
    
    /*** UTILS ***/
    
    MoMaFeature * processMoMaFeature( string name, Track * track, arma::mat * src ) {
        
        MoMaFeature * tmp = new MoMaFeature( name, track, src );
        cout << "MoMaFeature " << tmp->isInitialised() << endl;
        if ( tmp->isInitialised() ) {
            return tmp;
        } else {
            delete tmp;
            return NULL;
        }
    }

    
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
    
    MoMaBone getBoneByTail( std::string tailname, Track &track, int frame ) {
        int nID = getNodeID( tailname, track );
        if ( nID == -1 ) {
            MoMaBone mmb;
            mmb.exists = false;
            return mmb;
        }
        vector< std::pair<int,int> > * bones = ( vector< std::pair<int,int> > * ) track.boneList;
        for ( vector< std::pair<int,int> >::iterator itb = bones->begin(); itb != bones->end(); itb++ ) {
            if ( (*itb).second == nID ) {
                MoMaBone mmb;
                mmb.exists = true;
                mmb.headID = (*itb).first;
                mmb.tailID = nID;
                mmb.head = toVec3f( track.position.getData().slice( frame ).col( mmb.headID ) );
                mmb.tail = toVec3f( track.position.getData().slice( frame ).col( mmb.tailID  ) );
                return mmb;
            }
        }
        MoMaBone mmb;
        mmb.exists = false;
        return mmb;
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
            vector< std::pair<int,int> > * bones = ( vector< std::pair<int,int> > * ) track.boneList;
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
            vector< std::pair<int,int> > * bones = ( vector< std::pair<int,int> > * ) track.boneList;
            for ( vector< std::pair<int,int> >::iterator itb = bones->begin(); itb != bones->end(); itb++ ) {
                if ( (*itb).first == nID ) out.push_back( (*itb).second );
            }
        }
        return out;
    }
    
}
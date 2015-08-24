/**
 * @file mmOfUtils.h
 * @author Numediart
 * @date 2/04/15
 * @brief Utilitary functions to use MoMa in OF.
 *
 * Here go tools to use MoMa in OF@@.
 */

#ifndef __mmOfUtil__
#define __mmOfUtil__

#include <stdio.h>
#include <armadillo>
#include "ofMain.h"
#include "mmTrack.h"

namespace MoMa {

    class MoMaFeature {
    public:
        MoMaFeature( string name, Track * track, arma::mat * src );
        ~MoMaFeature();
        bool isInitialised() { return initialised; }
        bool hasValue( unsigned int nodeID );
        string getName() { return name; }
        float getRaw( unsigned int nodeID, unsigned int frame );
        float getNormalisedLocal( unsigned int nodeID, unsigned int frame );
        float getNormalisedGlobal( unsigned int nodeID, unsigned int frame );
        float getDelta( unsigned int nodeID, unsigned int frame );
    private:
        bool initialised;
        string name;
        Track * track;
        unsigned int rows;
        unsigned int cols;
        map< int, int > ids_map;
        arma::mat raw;
        arma::mat normalised_local; // normalisation per row
        arma::mat normalised_global;
        arma::mat delta;
        void process( arma::mat * src );
    };
    
    struct MoMaBone {
        bool exists;
        unsigned int headID;
        unsigned int tailID;
        ofVec3f head;
        ofVec3f tail;
    };
    
    /**
     * @brief Create a oF 3D vector from a 3-dim Armadillo vector
     *
     * Create a oF 3D vector from a 3-dim Armadillo vector
     *
     * @param data the arma vector
     * @return The ofVec3f with the input elements
     */
    ofVec3f toVec3f( arma::vec data );
    
    /**
     * @brief Create a oF Quaternion from a 4-dim Armadillo vector
     *
     * Create a oF Quaternion from a 4-dim Armadillo vector
     *
     * @param data the arma vector
     * @return The ofVec3f with the input elements
     */
    ofQuaternion toQuaternion( arma::vec data );
    
    // frankiezafe utils
    
    // structure info
    int getNodeID( std::string nodename, Track &track );
    int getNodeOriginID( std::string nodename, Track &track );
    std::vector< unsigned int > getNoneChildrensID( std::string nodename, Track &track );

    // data retrieval
    ofVec3f getNodePosition( std::string nodename, Track &track, int frame );
    ofQuaternion getNodeRotation( std::string nodename, Track &track, int frame );
    MoMaBone getBoneByTail( std::string tailname, Track &track, int frame  );
    
    // prepare feature (normalisations)
    MoMaFeature * processMoMaFeature( string name, Track * track, arma::mat * src );
    
}

#endif

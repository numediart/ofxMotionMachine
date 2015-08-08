#include "mmParser.h"

using namespace std;
using namespace MoMa;


Parser::Parser( string const &fName, Track *tr ) {

    track = tr;
    fileName = fName;

#ifdef _WIN32
    fileName = checkFileName( fileName );
#endif

    //set trackName
    /*string shortFName;
    char* tmp = new char[fileName.size()+1];
    strncpy(tmp, fileName.c_str(),fileName.size());
    tmp[fileName.size()]=NULL;
    char* tmpName;
    _splitpath(tmp,NULL,NULL,tmpName,NULL);
    shortFName=string(tmpName);
    track->setTrackName(shortFName);*/

    size_t sep = fileName.find_last_of("\\/");
    size_t dot = fileName.find_last_of(".");    

    int previousSize = track->nOfNodes();
    extension = fileName.substr( dot + 1) ;
    transform( extension.begin(), extension.end(), extension.begin(), ::tolower );

    if( extension == "txt" ) {        

        track->setFileName( fileName.substr( sep + 1, dot-sep-1 ) );

        FlatParser flatParser( fileName,track );

        // Check validity of lists of bones and nodes
        if( track->nOfNodes() != previousSize && previousSize != 0 ) {

            // different number of
            // markers => different labels
            if( track->hasNodeList == true ) {

                track->hasNodeList = false;
                delete track->nodeList;
            }

            //different number of
            // markers => different skeleton
            if( track->hasBoneList == true ) {

                track->hasBoneList = false;
                delete track->boneList;
            }
        }
    }

    else if( extension == "v3d" ) {

        track->setFileName( fileName.substr( sep + 1, dot-sep-1 ) );

        V3dParser v3dParser( fileName, track );

        // Check validity of list of bones
        if( track->nOfNodes() != previousSize &&
            previousSize != 0 && track->hasBoneList == true ) {

                // different number of
                // markers => different skeleton
                track->hasBoneList = false;
                delete track->boneList;
        }
        this->setJointOffsetRotation(tr);
    }

    else if( extension == "c3d" ) {

        track->setFileName( fileName.substr( sep + 1, dot-sep-1 ) );

        C3dParser c3dParser( fileName, track );

        // Check validity of list of bones
        if( track->nOfNodes() != previousSize &&
            previousSize !=0 && track->hasBoneList == true ) {

                // different number of
                // markers => different skeleton
                track->hasBoneList = false;
                delete track->boneList;
        }
    }

    else if( extension == "bvh" ) {

        track->setFileName( fileName.substr( sep + 1, dot-sep-1 ) );

        BvhParser::load( fileName, track );

        // Check validity of list of bones
        if( track->nOfNodes() != previousSize &&
            previousSize !=0 && track->hasBoneList == true ) {

                // different number of
                // markers => different skeleton
                track->hasBoneList = false;
                delete track->boneList;
        }
        this->setJointOffsetRotation(tr);
    }

    else if( extension == "bones" ) {
        track->bones( fileName );
        this->setJointOffsetRotation(tr);
    }

    else if( extension == "nodes" ) track->nodes( fileName );

    else cout << "Invalid file format" << endl;
}

string Parser::checkFileName( string const &fName ) {

    string tmp;

    // Added to manage special characters (accents
    // for example) in file path (extended ASCII table)

    for( int i=0; i<fName.size(); ++i ) {

        if( fName[i] != -61 ) {

            // Basic ASCII table
            tmp.push_back( fName[i] );

        } else {

            // Manage buggy special characters (extended ASCII character) the
            // special character is divided in two characters : -61 and X (between
            // -1 and -127); to get a valid character the conversion is c = X+64

            ++i; // tmp[i] was -61
            tmp.push_back( fName[i]+64 );
        }
    }

    return tmp;
}


bool Parser::setJointOffsetRotation(Track *tr) {
    
    tr->rotationOffset.resize(4,tr->rotation.nOfNodes());
    bool debug = false;
    if (!tr->boneList || !tr->hasNodeList || !tr->hasRotation || !tr->hasSynoList)
        return false;

    tr->rotationOffset.resize(4,tr->nOfNodes());
    Frame frame0 = tr->frame(0);
    arma::colvec frontalAxis;
    arma::colvec tempVec;

    tempVec<<frame0.node("LShoulder").position[0] <<frame0.node("LShoulder").position[1]<<frame0.node("LShoulder").position[2];
    frontalAxis<<frame0.node("RShoulder").position[0] <<frame0.node("RShoulder").position[1]<<frame0.node("RShoulder").position[2];
    frontalAxis=normalise(tempVec-frontalAxis);


    arma::colvec longAxis;

    tempVec.clear();

    tempVec<<frame0.node("Head").position[0] <<frame0.node("Head").position[1]<<frame0.node("Head").position[2];
    longAxis<<frame0.node("Pelvis").position[0] <<frame0.node("Pelvis").position[1]<<frame0.node("Pelvis").position[2];

    longAxis=normalise(tempVec-longAxis);
    arma::colvec sagAxis;
    sagAxis=arma::cross(frontalAxis,longAxis);
    if( debug ) std::cout<<"frontal"<<std::endl;
    if( debug ) std::cout<<frontalAxis<<std::endl;

    if( debug ) std::cout<<"long"<<std::endl;
    if( debug ) std::cout<<longAxis<<std::endl;
    if( debug ) std::cout<<"sag"<<std::endl;
    if( debug ) std::cout<<sagAxis  <<std::endl;

    for (int i=0;i<tr->boneList->size();i++){
        int orig=tr->boneList->at(i).first;
        int dest=tr->boneList->at(i).second;
        if( debug ) std::cout<<frame0.node(orig).name()<<"->"<<frame0.node(dest).name()<<std::endl;
        std::vector<float> val;
        arma::mat offsetMatrix;
        offsetMatrix.eye(3,3);
        arma::colvec tempVecX,tempVecY,tempVecZ;
        tempVecX<<frame0.node(dest).position[0]-frame0.node(orig).position[0]<<frame0.node(dest).position[1]-frame0.node(orig).position[1]<<frame0.node(dest).position[2]-frame0.node(orig).position[2];
        tempVecX=arma::normalise(tempVecX);

        if (std::abs(arma::dot(tempVecX,sagAxis))>std::abs(arma::dot(tempVecX,longAxis))&&std::abs(arma::dot(tempVecX,sagAxis))>std::abs(arma::dot(tempVecX,frontalAxis))){

            tempVecZ=arma::cross(tempVecX,frontalAxis);
            tempVecY=arma::cross(tempVecZ,tempVecX);

        }
        else if (std::abs(arma::dot(tempVecX,frontalAxis))>std::abs(arma::dot(tempVecX,longAxis))){

            tempVecY=arma::cross(longAxis,tempVecX);
            tempVecZ=arma::cross(tempVecX,tempVecY);

        }
        else if (arma::dot(tempVecX,longAxis)>0){

            tempVecZ=arma::cross(frontalAxis,tempVecX);
            tempVecY=arma::cross(tempVecZ,tempVecX);
        }
        else {
            tempVecZ=arma::cross(tempVecX,frontalAxis);
            tempVecY=arma::cross(tempVecZ,tempVecX);
        }

        offsetMatrix.col(0)=arma::normalise( tempVecX);
        offsetMatrix.col(1)=arma::normalise( tempVecY);
        offsetMatrix.col(2)=arma::normalise( tempVecZ);
        if( debug ) std::cout<<offsetMatrix<<std::endl;

        quaternion origQuat(frame0.node(orig).rotation);
        quaternion offsetQuat;
        offsetQuat.set(offsetMatrix);
        quaternion lquat(origQuat.inverse()*offsetQuat);

        if( debug ) std::cout<<lquat(0)<<" "<<lquat(1)<<" "<<lquat(2)<<" "<<lquat(3)<<std::endl;
        tr->rotationOffset.col(dest)=lquat;
    }

    return true;
}

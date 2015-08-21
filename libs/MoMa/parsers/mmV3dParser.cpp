#include "mmV3dParser.h"

using namespace std;
using namespace MoMa;

V3dParser::V3dParser( string const &fileName, Track *track ) {
    
    load( fileName, track );
    dim = 3; // 3D space
}

void V3dParser::load( string const &fileName, Track *track ) {
    
    ifstream v3dFile( fileName.c_str() );
    
    if( !v3dFile.is_open() ) {
        
        cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }
    
    // Read the file ones to get
    // the number of lines or frames
    
    unsigned int nbOfFrames = 0;
    
    // Skip header ( 5 lines )
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 1
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 2
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 3
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 4
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 5
    
    while( v3dFile.good() ) {
        
        getline( v3dFile, thisLine );
        
        if( thisLine != "" && thisLine != " "
        && thisLine != "\t" && thisLine != "\n" ) {
            
            // We count the number
            // of line in file
            
            ++nbOfFrames;
        }
    }
    
    v3dFile.clear(); // Return to beginning
    v3dFile.seekg(v3dFile.beg); // of the file
    
    track->clear(); // Clear the track before
    
    nodeList = new NodeList(); // We create a nodeList,
    track->nodeList = nodeList; // add it to the track
    track->hasNodeList = true; // and tell everybody
    
    // Two more lines to skip
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 1
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 2
    
    thisStream << thisLine; // We store the line in a string stream
    
    while( thisStream.good() ) {
        
        // We save the list of tags
        string tag; thisStream >> tag;
        rawJoint.push_back( tag );
    }
    
    if( rawJoint[3] == rawJoint[2]
    && rawJoint[6] != rawJoint[2] ) {
        
        // Remark : can bug if different
        // nodes have the same name
        
        track->hasRotation = true;
        dim = 6; // 6 DOF here
        
    } else if( rawJoint[3] == rawJoint[2]
    && rawJoint[6] == rawJoint[2] ) {
        
        track->hasRotation = true;
        dim = 3+9; // 3D + rot matrix
    
    } else {
        
        track->hasRotation = false;
        dim = 3; // Just 3D points
    }
    
    // We resize and fill the track nodeList
    unsigned int nbOfNodes = rawJoint.size()/dim;
    track->nodeList->resize( nbOfNodes );
    
    for( int r=0, n=0; n<nbOfNodes; r+=dim, n++  ) {
        
        track->nodeList->at(n) = rawJoint[r];
    }
    
    // And we skip 3 more lines here
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 3
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 4
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 5
    
    thisStream.clear(); // Clear the current stream
    thisStream << thisLine; // and put the new line
    
    while( thisStream.good() ) {
        
        // We get the current dim tag
        string tag; thisStream >> tag;
        
        if( tag.compare( "X" ) == 0 ) {
            
            axisIndex.push_back( X );
            
        } else if( tag.compare( "Y" ) == 0 ) {
            
            axisIndex.push_back( Y );
            
        } else if( tag.compare( "Z" ) == 0 ) {
            
            axisIndex.push_back( Z );
            
        } else if( tag.compare("ITEM") == 0 ) {
            
            // Skip this tag
            
        } else {
            
            axisIndex.push_back( -1 );
        }
    }
    
    track->position.getRefData().resize( 3, nbOfNodes, nbOfFrames );
    
    if( track->hasRotation ) {
        
        track->rotation.getRefData().resize( 4, nbOfNodes, nbOfFrames );
        track->rotationOffset.resize( 4, nbOfNodes );
    }
    
    unsigned int frameCpt = 0; // Init frame count
    
    int index;
    
    while( v3dFile.good() ) {
        
        getline( v3dFile, thisLine );
        
        if( thisLine != "" && thisLine != " " &&
        thisLine != "\t" && thisLine != "\n" ) {
            
            arma::mat posMat( 3, nbOfNodes );
            arma::mat rotMat( 4, nbOfNodes );
            
            thisStream.clear(); // Clear and grab
            thisStream << thisLine; // a new line
            
            if( thisStream.good() ) {
                
                thisStream >> index; index--;
                unsigned int nodeCpt = 0;
                
                for( int k=0; k<axisIndex.size(); k+=dim ) {
                    
                    string value[12];
                    
                    thisStream >> value[0];
                    thisStream >> value[1];
                    thisStream >> value[2];
                    
                    if( dim == 6 ) {
                        
                        thisStream >> value[3];
                        thisStream >> value[4];
                        thisStream >> value[5];
                    }
                    
                    if( dim == 12 ) {
                        
                        thisStream >> value[3];
                        thisStream >> value[4];
                        thisStream >> value[5];
                        thisStream >> value[6];
                        thisStream >> value[7];
                        thisStream >> value[8];
                        thisStream >> value[9];
                        thisStream >> value[10];
                        thisStream >> value[11];
                    }
                    
                    if( value[0] == "NaN" || atof( value[0].c_str() ) > MOMAINF ) {
                        
                        // Data are ignored and the matrices
                        // take arma's NaNs as positions/rotations
                        
                        posMat.col( nodeCpt ) = arma::ones(3) * arma::datum::nan;
                        rotMat.col( nodeCpt ) = arma::ones(4) * arma::datum::nan;
                    
                    } else {
                        
                        posMat( axisIndex[k], nodeCpt ) = atof( value[0].c_str() );
                        posMat( axisIndex[k+1], nodeCpt ) = atof( value[1].c_str() );
                        posMat( axisIndex[k+2], nodeCpt ) = atof( value[2].c_str() );
                        
                        posMat( axisIndex[k], nodeCpt ) *= 1000;
                        posMat( axisIndex[k+1], nodeCpt ) *= 1000;
                        posMat( axisIndex[k+2], nodeCpt ) *= 1000;
                        
                        if( dim == 6 ) {
                            
                            float roll = atof( value[3].c_str() ); // Rotation autour de l'axe X
                            float pitch = atof( value[4].c_str() ); // Rotation autour de l'axe Y
                            float yaw = atof( value[5].c_str() ); // Rotation autour de l'axe Z
                            
                            arma::vec axis1, axis2, axis3;
                            
                            axis1 << 1 << 0 << 0;
                            axis2 << 0 << 1 << 0;
                            axis3 << 0 << 0 << 1;
                            
                            quat.makeRotate( yaw, axis3, pitch, axis2, roll, axis1 );
                            rotMat.col( nodeCpt ) = quat; // Achieve the rotation
                        }
                        
                        if( dim == 12 ) {
                            
                            arma::mat Rot;
                            
                            Rot
                            << atof( value[3].c_str() ) << atof( value[6].c_str() ) << atof( value[9].c_str() ) << arma::endr
                            << atof( value[4].c_str() ) << atof( value[7].c_str() ) << atof( value[10].c_str() ) << arma::endr
                            << atof( value[5].c_str() ) << atof( value[8].c_str() ) << atof( value[11].c_str() ) << arma::endr;
                            
                            quat.set( Rot ); rotMat.col( nodeCpt ) = quat; // Achieve rotation from the full matrix
                        }
                    }
                    
                    nodeCpt++;
                }
                
                track->position.getRefData().slice( frameCpt ) = posMat; // Put frames at location
                if( track->hasRotation ) track->rotation.getRefData().slice( frameCpt ) = rotMat;
                
                frameCpt++;
            }
        }
    }
    
    track->setFrameRate( 177 ); // TODO to define to look for it somewhere
    track->hasOrigNodeRot_as_boneRot=true;
    v3dFile.close();
}

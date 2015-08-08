#include "mmV3dParser.h"

using namespace std;
using namespace MoMa;

V3dParser::V3dParser( string const &fileName, Track *track ) {

    dim=3;
    load( fileName, track );

}

void V3dParser::load( string const &fileName, Track *track ) {

    ifstream v3dFile( fileName.c_str() );

    if( !v3dFile.is_open() ) {

        cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }
    
    //Read the file ones to get the number of lines or frames
    unsigned int nbOfFrames = 0;

    //Header
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 1
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 2
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 3
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 4
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller à la ligne 5

    while( v3dFile.good() ) {

        getline( v3dFile, thisLine );
        if( thisLine != "" && thisLine != " " && thisLine != "\t" && thisLine != "\n" ) {

            ++nbOfFrames; 
        }
    }

    //Or query index of last frame :
    /*int nOfFrames = 0;

    //Header
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 1
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 2
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 3
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 4
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller à la ligne 5

    while( getline( v3dFile, thisLine ) ) {

    if( thisLine != "" && thisLine != " " && thisLine != "\t" && thisLine != "\n" ) {

    thisStream << thisLine;
    thisStream >> nOfFrames;
    thisStream.clear();
    }
    }*/


    //Return to beginning of file
    v3dFile.clear();
    v3dFile.seekg(v3dFile.beg);

    track->clear();

    nodeList = new NodeList();
    track->nodeList = nodeList;
    track->hasNodeList = true;
    
    // cout << "File is opened" << endl;

    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 1
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 2

    thisStream << thisLine;

    while( thisStream.good() ) {

        string tag;
        thisStream >> tag;

        rawJoint.push_back( tag );
    }

    if( rawJoint[3] == rawJoint[2] && rawJoint[6] != rawJoint[2] ) // 6DOF (x y z yaw pitch roll) (remark : can bug if different nodes have the same name)
    {
        dim = 6;
        // cout << "data format : 6DOF" << endl;
        track->hasRotation = true;
    }
    else if( rawJoint[3] == rawJoint[2] && rawJoint[6] == rawJoint[2] )
    { // x y z rotation matrix
        dim = 3+9;
        // cout << "data format : 3D + rotation matrix" << endl;
        track->hasRotation = true;
    }
    else
    {
        dim = 3;
        // cout << "data format : 3D" << endl;
    }

    nodeList->resize( rawJoint.size()/dim );

    unsigned int nbOfNodes=rawJoint.size()/dim;
    for( int r=0, n=0; n<rawJoint.size()/dim; r+=dim, n++  ) {

        nodeList->at(n) = rawJoint[r];
    }

    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 3
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller a la ligne 4
    if( v3dFile.good() ) getline( v3dFile, thisLine ); // Aller à la ligne 5

    thisStream.clear();
    thisStream << thisLine;

    while( thisStream.good() ) {

        string tag; // existe seulement dans la boucle?
        thisStream >> tag;

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

    int index;

    if(dim==3)
    {
        track->hasRotation=false;
    }
    if(dim==6)
    {
        track->hasRotation=true;
    }
    if(dim==12)
    {
        track->hasRotation=true;
    }

    //track->position.getRefData().resize(3,nbOfNodes,1000);
    track->position.getRefData().resize(3,nbOfNodes,nbOfFrames);

    if (track->hasRotation) {

        //track->rotation.getRefData().resize(4,nbOfNodes,1000);
        track->rotation.getRefData().resize(4,nbOfNodes,nbOfFrames);
        track->rotationOffset.resize(4,nbOfNodes);
    }

    unsigned int frameCpt=0;
    while( v3dFile.good() ) {

        getline( v3dFile, thisLine );

        if( thisLine != "" && thisLine != " "
            && thisLine != "\t" && thisLine != "\n" ) {

                arma::mat posMat(3,nbOfNodes);
                arma::mat rotMat(4,nbOfNodes);
                //Frame tempFrame;
                //  if(dim==6||dim==12) tempFrame.setRotationFlag(true);
                thisStream.clear();
                thisStream << thisLine;

                if( thisStream.good() ) {

                    thisStream >> index; index--;
                    unsigned int nodeCpt=0;

                    for( int k=0; k<axisIndex.size(); k+=dim ) {


                        //Node tempNode;
                        string value[12];

                        thisStream >> value[0];
                        thisStream >> value[1];
                        thisStream >> value[2];

                        if(dim==6)
                        {
                            //tempNode.setRotationFlag(true);
                            thisStream >> value[3];
                            thisStream >> value[4];
                            thisStream >> value[5];
                        }
                        if(dim==12)
                        {
                            //tempNode.setRotationFlag(true);
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

                        if( value[0] == "NaN" ||  atof( value[0].c_str() ) > 100000000000 ) {

                            // Data are ignored and the
                            // tempNode keeps arma's nans for positions

                            //put rotation to nan too
                            //tempNode.setRotation(arma::datum::nan,arma::datum::nan,arma::datum::nan,arma::datum::nan);
                            rotMat.col(nodeCpt)=arma::ones(4)*arma::datum::nan;
                        }
                        else {

                            //                        tempNode.position( axisIndex[k] ) = atof( value[0].c_str() );
                            //                        tempNode.position( axisIndex[k+1] ) = atof( value[1].c_str() );
                            //                        tempNode.position( axisIndex[k+2] ) = atof( value[2].c_str() );

                            posMat(axisIndex[k],nodeCpt)=atof( value[0].c_str() );
                            posMat(axisIndex[k+1],nodeCpt)=atof( value[1].c_str() );
                            posMat(axisIndex[k+2],nodeCpt)=atof( value[2].c_str() );

                            //                        tempNode.position( axisIndex[k] ) *= 1000; // conversion to mm
                            //                        tempNode.position( axisIndex[k+1] ) *= 1000; // conversion to mm
                            //                        tempNode.position( axisIndex[k+2] ) *= 1000; // conversion to mm

                            posMat(axisIndex[k],nodeCpt)*=1000;
                            posMat(axisIndex[k+1],nodeCpt)*=1000;
                            posMat(axisIndex[k+2],nodeCpt)*=1000;

                            if(dim==6)
                            {

                                float roll = atof( value[3].c_str() ); // Rotation autour de l'axe X
                                float pitch = atof( value[4].c_str() ); // Rotation autour de l'axe Y
                                float yaw = atof( value[5].c_str() ); // Rotation autour de l'axe Z

                                arma::vec axis1, axis2, axis3;
                                axis1 << 1 << 0 << 0;
                                axis2 << 0 << 1 << 0;
                                axis3 << 0 << 0 << 1;

                                //quat.makeRotate( roll, axis1, pitch, axis2, yaw, axis3);
                                quat.makeRotate( yaw, axis3, pitch, axis2, roll, axis1); //inversed order!
                                //tempNode.rotation=quat;
                                rotMat.col(nodeCpt)=quat;

                                /*arma::mat Rot;
                                Rot
                                <<   0<<   0<<   1 << arma::endr
                                <<   0<<   1<<   0 << arma::endr
                                <<   -1<<   0<<   0 << arma::endr;
                                quat.set(Rot);
                                //cout << quat << endl;
                                tempNode.setOffsetRotation(quat(0),quat(1),quat(2),quat(3));*/
                            }

                            if(dim==12)
                            {

                                arma::mat Rot;
                                Rot
                                    <<   atof( value[3].c_str() )<<   atof( value[6].c_str() )<<   atof( value[9].c_str() ) << arma::endr
                                    <<   atof( value[4].c_str() )<<   atof( value[7].c_str() )<<   atof( value[10].c_str() ) << arma::endr
                                    <<   atof( value[5].c_str() )<<   atof( value[8].c_str() )<<   atof( value[11].c_str() ) << arma::endr;

                                quat.set(Rot);
                                //tempNode.rotation=quat;
                                rotMat.col(nodeCpt)=quat;

                            }

                        }

                        //tempFrame.push( tempNode );
                        //std::cout<<nodeCpt<<std::endl<<posMat.col(nodeCpt)<<std::endl;
                        nodeCpt++;
                    }
                    if (frameCpt >=track->position.getRefData().n_slices){
                        track->position.getRefData().resize(3,nbOfNodes,track->position.getRefData().n_slices+1000);
                        if (track->hasRotation)
                            track->rotation.getRefData().resize(4,nbOfNodes,track->rotation.getRefData().n_slices+1000);
                    }

                    track->position.getRefData().slice(frameCpt)=posMat;
                    if (track->hasRotation)
                        track->rotation.getRefData().slice(frameCpt)=rotMat;

                    //track->push( tempFrame );
                    track->position.setFrameRate(177);
                    if (track->hasRotation)
                        track->rotation.setFrameRate(177);
                    track->frameRate = 177; // TODO to define to look for
                    //std::cout<<frameCpt<<std::endl<<track->position.getRefData().slice(frameCpt)<<std::endl;
                    frameCpt++;
                }
        }
    }
    track->position.getRefData().resize(3,nbOfNodes,frameCpt);
    track->rotation.getRefData().resize(4,nbOfNodes,frameCpt);

    v3dFile.close();
}

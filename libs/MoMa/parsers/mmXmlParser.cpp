#include "mmXmlParser.h"
#include "mmQuaternion.h"

using namespace std;
using namespace MoMa;

XmlParser::XmlParser( std::string const &fileName, MoMa::Track *track ) {

    load( fileName, track );
}

void XmlParser::load( string const &fileName, Track *track ) {
    
    ifstream datFile( fileName.c_str() ); // Open file
    
    if( !datFile.is_open() ) {
        
        std::cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }
    
    track->clearData();
	
	unsigned int nbFrames = 0;
	unsigned int nbNodes = 0;
	bool bls = true;

    while ( datFile.good() ) {

		//string name;
        string curLine;
        stringstream curStrm;
        vector<string> rawData;
        std::getline( datFile, curLine );

		if ( curLine.find( "</JointPositions>" ) != string::npos )
			bls = false;
		
		if ( curLine.find( "<KinectSkeletalAnimation>" ) != string::npos ) 
			nbFrames++;	

		else if ( bls && curLine.find( "Joint " ) != string::npos )
			nbNodes++;
	}

    datFile.clear() ;
	datFile.seekg( 0, ios::beg ) ;
	arma::cube pos( 3, nbNodes, nbFrames );
	arma::cube rot( 4, nbNodes, nbFrames );

	unsigned int frameId = 0;
	unsigned int nodeId = 0;

	bool kinectSkeletalAnimation_bls = false;
	bool frame_bls = false;
	bool jointPosition_bls = false;
	bool jointRotation_bls = false;

    while ( datFile.good() ) {

        string curLine;
        stringstream curStrm;
        
        std::getline( datFile, curLine );

		if ( curLine.find( "<KinectSkeletalAnimation>" ) != string::npos )
			kinectSkeletalAnimation_bls = true;

		else if ( curLine.find( "</KinectSkeletalAnimation>" ) != string::npos ) {
			
			kinectSkeletalAnimation_bls = false;
			frameId++;
		}

		else if ( curLine.find( "Frame" ) != string::npos )
			frame_bls = true;

		else if ( curLine.find( "<JointPositions>" ) != string::npos )
			jointPosition_bls = true;

		else if ( curLine.find( "</JointPositions>" ) != string::npos ) {
		
			jointPosition_bls = false;
			nodeId = 0;
		}

		else if ( curLine.find( "<JointRotationsHierarchical>" ) != string::npos )
			jointRotation_bls = true;

		else if ( curLine.find( "</JointRotationsHierarchical>" ) != string::npos ) {
		
			jointRotation_bls = false;
			nodeId = 0;
		}

		if ( kinectSkeletalAnimation_bls ) {

			if ( jointPosition_bls ) {

				if ( curLine.find("Joint ") != string::npos ) {
			
					curStrm.clear();
					curStrm << curLine;

					string value[5];
					curStrm >> value[0];
					curStrm >> value[1];
					curStrm >> value[2];
					curStrm >> value[3];
					curStrm >> value[4];

					value[1].replace( value[1].begin(), value[1].begin() + 3, "" );
					value[1].replace( value[1].end()-1, value[1].end(), "" );
					std::replace( value[1].begin(), value[1].end(), ',','.' );

					value[2].replace( value[2].begin(), value[2].begin() + 3, "" );
					value[2].replace( value[2].end()-1, value[2].end(), "" );
					std::replace( value[2].begin(), value[2].end(), ',','.' );

					value[3].replace( value[3].begin(), value[3].begin() + 3, "" );
					value[3].replace( value[3].end()-1, value[3].end(), "" );
					std::replace( value[3].begin(), value[3].end(), ',','.' );

					pos( X, nodeId, frameId ) = -atof( value[3].c_str() );
					pos( Y, nodeId, frameId ) = -atof( value[1].c_str() );
					pos( Z, nodeId, frameId ) = atof( value[2].c_str() );

					nodeId++;
				}
			}

			if ( jointRotation_bls ) {

				if ( curLine.find("Joint ") != string::npos ) {
			
					curStrm.clear();
					curStrm << curLine;

					string value[6];
					curStrm >> value[0];
					curStrm >> value[1];
					curStrm >> value[2];
					curStrm >> value[3];
					curStrm >> value[4];
					curStrm >> value[5];

					value[1].replace( value[1].begin(), value[1].begin() + 3, "" );
					value[1].replace( value[1].end()-1, value[1].end(), "" );
					std::replace( value[1].begin(), value[1].end(), ',','.' );

					value[2].replace( value[2].begin(), value[2].begin() + 3, "" );
					value[2].replace( value[2].end()-1, value[2].end(), "" );
					std::replace( value[2].begin(), value[2].end(), ',','.' );

					value[3].replace( value[3].begin(), value[3].begin() + 3, "" );
					value[3].replace( value[3].end()-1, value[3].end(), "" );
					std::replace( value[3].begin(), value[3].end(), ',','.' );

					value[4].replace( value[4].begin(), value[4].begin() + 3, "" );
					value[4].replace( value[4].end()-1, value[4].end(), "" );
					std::replace( value[4].begin(), value[4].end(), ',','.' );

					rot( qW, nodeId, frameId ) = atof( value[1].c_str() );
					rot( qX, nodeId, frameId ) = -atof( value[4].c_str() );
					rot( qY, nodeId, frameId ) = -atof( value[2].c_str() );
					rot( qZ, nodeId, frameId ) = atof( value[3].c_str() );

					nodeId++;
				}
			}

		}
		
	}
	
	// Rotations part needs to be finished [ How to push rotations to the track? ]

    int nbBones = nbNodes;
    if( track->hasBoneList ) {

        nbBones = track->boneList->size();
        arma::cube sortedRot( 4, nbBones, nbFrames );
        for( int frameId = 0; frameId < nbFrames; frameId++ ) {
            arma::mat rotTemp( 4, nbBones );
            for( boneMapType::iterator it = track->boneList->begin(); it != track->boneList->end(); it++ ) {

                rotTemp.col( it->second.boneId ) = rot.slice(frameId).col( it->second.jointChildren[0] );
            }
            sortedRot.slice( frameId ) = rotTemp;
        }
        track->hasRotation = true;
        track->rotation.setData( 30, sortedRot );
    }
    else
        track->hasRotation = false;

    

	track->position.setData( 30, pos );
    track->setFrameRate(30);

    datFile.close();
   
}

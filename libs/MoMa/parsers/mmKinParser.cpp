#include "mmKinParser.h"
#include "mmQuaternion.h"
using namespace std;
using namespace MoMa;

KinParser::KinParser( std::string const &fileName, MoMa::Track *track ) {
    load( fileName, track );
}

void KinParser::load( string const &fileName, Track *track ) {
    
    ifstream datFile( fileName.c_str() ); // Open file
    
    if( !datFile.is_open() ) {
        
        std::cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }
    
    track->clearData();
	
	unsigned int nbFrames=0;
	unsigned int nbNodes=0;

    while ( datFile.good() ) {
		string name;
        string curLine;
        stringstream curStrm;
        vector<string> rawData;
        std::getline( datFile, curLine );
		
		unsigned int lNbNodes=0;
        if( curLine != "" && curLine != " "
			&& curLine != "\t" && curLine != "\n" ) {
				curStrm.clear();
				curStrm << curLine;
				nbFrames++;
				while( curStrm.good() ) {
					string value[7];
					curStrm >> value[0];
					if (value[0]==std::string(""))
						continue;
					curStrm >> value[1];
					curStrm >> value[2];
					curStrm >> value[3];
					curStrm >> value[4];
					curStrm >> value[5];
					curStrm >> value[6];
					lNbNodes++;

				}	
		}
		nbNodes=nbNodes>lNbNodes?nbNodes:lNbNodes;
	}
    datFile.clear() ;
	datFile.seekg(0, ios::beg) ;
	arma::cube pos(3,nbNodes,nbFrames);
	int nbBones = nbNodes;
	if (track->hasBoneList)
		nbBones = track->boneList->size();
	arma::cube sortedRot(4, nbBones, nbFrames);
	unsigned int frameId=0;
    while ( datFile.good() ) {
		string name;
        string curLine;
        stringstream curStrm;
        vector<string> rawData;
        
        std::getline( datFile, curLine );

        if( curLine != "" && curLine != " "
        && curLine != "\t" && curLine != "\n" ) {
            
            //Frame oneFrame;

			arma::mat rot(4, nbNodes);//size nbNode cause kinect model give an orientation for each node (some orientation are fake)
            curStrm.clear();
            curStrm << curLine;
            unsigned int nodeId=0;
            while( curStrm.good() ) {
                
                //Node oneNode;
                string value[7];
				curStrm >> value[0];
                if (value[0]==std::string(""))
                    continue;
				curStrm >> value[1];
				curStrm >> value[2];
				curStrm >> value[3];
				curStrm >> value[4];
				curStrm >> value[5];
				curStrm >> value[6];
				//curStrm >> value[7];
                
				if ((value[0] == "NaN" && value[1] == "NaN" && value[2] == "NaN" && value[3] == "NaN" && value[4] == "NaN" && value[5] == "NaN" && value[6] == "NaN" && value[7] == "NaN" ) ||
                ( atof(value[0].c_str()) > 100000000000 && atof( value[0].c_str() ) >
                100000000000 && atof( value[0].c_str()) > 100000000000 ) ) {
                       
                    // Data are ignored and the oneNode stays with ARMA NaNs
                    
                } 
				else {
                    

                    pos(0,nodeId,frameId) = atof( value[0].c_str() );
                    pos(1,nodeId,frameId) = atof( value[1].c_str() );
                    pos(2,nodeId,frameId) = atof( value[2].c_str() );
                    rot(0,nodeId) =atof( value[3].c_str() );
                    rot(1,nodeId)=atof( value[4].c_str() );
                    rot(2,nodeId)=atof( value[5].c_str() );
                    rot(3,nodeId)=atof( value[6].c_str() );
                    /*
					if (qw==1||(qx==0&&qy==0&&qz==0)){
                        oneNode.setRotation( arma::datum::nan,arma::datum::nan,arma::datum::nan,arma::datum::nan);
                    }
                    else
                            oneNode.setRotation( qx,qy,qz,qw);
					//oneNode.rotation(qX) = atof( value[3].c_str() );
					//oneNode.rotation(qY) = atof( value[4].c_str() );
					//oneNode.rotation(qZ) = atof( value[5].c_str() );
					//oneNode.rotation(qW) = atof( value[6].c_str() );
					*/

                }
				nodeId++;
                //oneFrame.push( oneNode );
				
            }
			//oneFrame.setRotationFlag( track->hasRotation );
            if( track->hasBoneList ) {
                arma::mat rotTemp( 4, nbBones );
                for( boneMapType::iterator it = track->boneList->begin(); it != track->boneList->end(); it++ ) {

                    rotTemp.col( it->second.boneId ) = rot.col( it->second.jointChildren[0] );
                }
                sortedRot.slice( frameId ) = rotTemp;
            }
			frameId++;
		}
	}

	track->position.setData(30,pos);
	track->rotation.setData(30, sortedRot);
	track->hasRotation=true;
	
	//track->hasOrigNodeRot_as_boneRot=false;
    datFile.close();
   
}

#include "mmKinParser.h"

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
    
    track->clear();
    while ( datFile.good() ) {
		string name;
        string curLine;
        stringstream curStrm;
        vector<string> rawData;
        
        std::getline( datFile, curLine );

        if( curLine != "" && curLine != " "
        && curLine != "\t" && curLine != "\n" ) {
            
            Frame oneFrame;
            
            curStrm.clear();
            curStrm << curLine;
            
            while( curStrm.good() ) {
                
                Node oneNode;
                string value[8];
				curStrm >> value[0];
				curStrm >> value[1];
				curStrm >> value[2];
				curStrm >> value[3];
				curStrm >> value[4];
				curStrm >> value[5];
				curStrm >> value[6];
				curStrm >> value[7];
                
				if ((value[0] == "NaN" && value[1] == "NaN" && value[2] == "NaN" && value[3] == "NaN" && value[4] == "NaN" && value[5] == "NaN" && value[6] == "NaN" && value[7] == "NaN" ) ||
                ( atof(value[0].c_str()) > 100000000000 && atof( value[0].c_str() ) >
                100000000000 && atof( value[0].c_str()) > 100000000000 ) ) {
                       
                    // Data are ignored and the oneNode stays with ARMA NaNs
                    
                } else {
                       
                    oneNode.position(X) = atof( value[0].c_str() );
                    oneNode.position(Y) = atof( value[1].c_str() );
                    oneNode.position(Z) = atof( value[2].c_str() );
					oneNode.setRotation( atof( value[3].c_str() ), atof( value[4].c_str() ), atof( value[5].c_str() ), atof( value[6].c_str() ) );
					//oneNode.rotation(qX) = atof( value[3].c_str() );
					//oneNode.rotation(qY) = atof( value[4].c_str() );
					//oneNode.rotation(qZ) = atof( value[5].c_str() );
					//oneNode.rotation(qW) = atof( value[6].c_str() );
                }

                oneFrame.push( oneNode );
				
            }
			//oneFrame.setRotationFlag( track->hasRotation );
            oneFrame.hasNodeList = track->hasNodeList;
            oneFrame.nodeList = track->nodeList;
            
            oneFrame.hasBoneList = track->hasBoneList;
            oneFrame.boneList = track->boneList;
            
            track->push( oneFrame );
        }
    }
    
    datFile.close();
}

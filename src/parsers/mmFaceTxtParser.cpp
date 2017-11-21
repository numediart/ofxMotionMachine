#include "mmFaceTxtParser.h"
#include <string>

using namespace std;
using namespace MoMa;

FaceTxtParser::FaceTxtParser( std::string const &fileName, MoMa::Track *track ) {

    load( fileName, track );
}

void FaceTxtParser::load( string const &fileName, Track *track ) {

    ifstream datFile( fileName.c_str() ); // Open file

    if( !datFile.is_open() ) {

        cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }

    track->clearData();

    unsigned int nbFrames=0,nbNodes=0;
	{
		string curLine;
		stringstream curStrm;
		vector<string> rawData;

		getline(datFile, curLine);
		getline(datFile, curLine);
	}
    while ( datFile.good() ) {

        string curLine;
        stringstream curStrm;

        getline( datFile, curLine );

        if( curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n" ) {
            curStrm.clear();
            curStrm << curLine;
            if (nbFrames==0){
                nbNodes=std::count( curLine.begin(), curLine.end(), ' ' )/3;
            }
            nbFrames++;
        }
    }
    datFile.clear() ;
    datFile.seekg(0, ios::beg) ;

	{//
		string curLine;
		stringstream curStrm;//First line with node names
		getline(datFile, curLine);
		if (curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n") {
			curStrm.clear();
			curStrm << curLine;
			string tempString;
			if (curStrm.good())
				curStrm >> tempString;//Frame#
			if (curStrm.good())
				curStrm >> tempString;//Time
			track->nodeList = std::shared_ptr<NodeList>(new NodeList);
			int cptNodes = 0;
			while (curStrm.good()) {
				string value;
				curStrm >> value;
				if (value != "NaN"){
					track->nodeList->insert(pair<string, int>(value, cptNodes));//NodeName
					cptNodes++;
				}
				if (cptNodes >= nbNodes)
					break;
			}
		}
		getline(datFile, curLine);
	}
	if (track->nodeList&&track->nodeList->size()>0)
		track->hasNodeList = true;
    arma::cube positionData(3,nbNodes,nbFrames);
    unsigned int cptFrames=0;
    while ( datFile.good() ) {

        string curLine;
        stringstream curStrm;
        vector<string> rawData;

        getline( datFile, curLine );

        if( curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n" ) {

            Frame oneFrame;

            curStrm.clear();
            curStrm << curLine;
			unsigned int cptNodes = 0;
			string cptNodesFile ;
			string time;
			curStrm >> cptNodesFile;//FrameId
			curStrm >> time;
			if (cptFrames==0)
				track->setFrameRate(1.f/atof(time.c_str()));
            while( curStrm.good() ) {

                //Node oneNode;

                string value[3];

                curStrm >> value[0];
                curStrm >> value[1];
                curStrm >> value[2];

                if( ( value[0] == "NaN" && value[1] == "NaN" && value[2] == "NaN" ) ||
                    ( atof(value[0].c_str()) > MOMAINF && atof( value[0].c_str() ) >
                    MOMAINF && atof( value[0].c_str()) > MOMAINF ) ) {
					positionData(X, cptNodes, cptFrames) = arma::datum::nan;
					positionData(Y, cptNodes, cptFrames) = arma::datum::nan;
					positionData(Z, cptNodes, cptFrames) = arma::datum::nan;

                        // Data are ignored and the oneNode stays with ARMA NaNs

                } else {

                    // oneNode.position(X) = atof( value[0].c_str() );
                    // oneNode.position(Y) = atof( value[1].c_str() );
                    // oneNode.position(Z) = atof( value[2].c_str() );
                    positionData(X,cptNodes,cptFrames)= atof( value[0].c_str() );
                    positionData(Y,cptNodes,cptFrames) =atof( value[1].c_str() );
                    positionData(Z,cptNodes,cptFrames) = atof( value[2].c_str() );

                }
				cptNodes++;
                if( cptNodes >= nbNodes )
                    break;
            }
            cptFrames++;
            //            track->push( oneFrame );
        }
    }

    track->position.setData( track->frameRate(), positionData );

    datFile.close();
}

#include "mmFaceCsvParser.h"

using namespace std;
using namespace MoMa;

FaceCsvParser::FaceCsvParser( std::string const &fileName, MoMa::Track *track ) {

    load( fileName, track );
}

void FaceCsvParser::load( string const &fileName, Track *track ) {

    ifstream datFile( fileName.c_str() ); // Open file

    if( !datFile.is_open() ) {

        cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }

    track->clearData();

	unsigned int nbFrames = 0, nbNodes = 0, nbChannels = 0;
	int nodesXId = -1, nodesYId = -1, nodesZId = -1;
	{	//Header
		string curLine;
		stringstream curStrm;
		vector<string> rawData;
		getline(datFile, curLine);
		curStrm.clear();
		curStrm << curLine;
		int cptNodesX = 0, cptNodesY = 0, cptNodesZ = 0;
		while (curStrm.good()) {
			string value;
			curStrm >> value;
			value = value.substr(0, value.find_first_of(","));
			if (value.find("X_") != std::string::npos) {
				if (nodesXId == -1)
					nodesXId= nbChannels;
				cptNodesX++;
			}
			if (value.find("Y_") != std::string::npos) {
				if (nodesYId == -1)
					nodesYId = nbChannels;
				cptNodesY++;
			}
			if (value.find("Z_") != std::string::npos) {
				if (nodesZId == -1)
					nodesZId = nbChannels;
				cptNodesZ++;
			}
			nbChannels++;
		}
		if (cptNodesX != cptNodesY || cptNodesX != cptNodesZ) {
			std::cout << "FaceCsvParser::load the number of chan corresponding to 3D point is not good" << std::endl;
			return;
		}
		nbNodes = cptNodesX;

	}

    while ( datFile.good() ) {

        string curLine;
        stringstream curStrm;
        vector<string> rawData;

		getline(datFile, curLine);

        if( curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n" ) {
            curStrm.clear();
            curStrm << curLine;
			std::vector<string> nodeNameVector;
			int chanCpt = 0;
			while (curStrm.good()) {
				string value;
				curStrm >> value;
				chanCpt++;
			}
			
            if (chanCpt!=nbChannels) {
				std::cout << "FaceCsvParser::load, the number of chan corresponding to 3D point is not good"<<std::endl;
				return;
			}

            nbFrames++;
        }
    }
    datFile.clear() ;
    datFile.seekg(0, ios::beg) ;
    
    
    arma::cube positionData(3,nbNodes,nbFrames);
    unsigned int cptFrames=0;
	{	//Header
		string curLine;
		stringstream curStrm;//First line with node names
		getline(datFile, curLine);
		if (curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n") {
			curStrm.clear();
			curStrm << curLine;
			track->nodeList = std::shared_ptr<NodeList>(new NodeList);
			int cptNodes = 0;
			std::vector<string> nodeNameVector;
			while (curStrm.good()) {
				string value;
				curStrm >> value;
				value = value.substr(0, value.find_first_of(","));
				if (value != "NaN") {
					nodeNameVector.push_back(value);
				}
			}
			for (int i = nodesXId; i < nodesXId+ nbNodes; i++) {
				nodeNameVector[i] = nodeNameVector[i].substr(nodeNameVector[i].find("_") + 1);
				track->nodeList->insert(pair<string, int>(nodeNameVector[i], i-152));//NodeName
			}
		}

	}

	if (track->nodeList&&track->nodeList->size()>0)
		track->hasNodeList = true;
	while ( datFile.good() ) {

        string curLine;
        stringstream curStrm;
        vector<string> rawData;

        getline( datFile, curLine );

        if( curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n" ) {

            Frame oneFrame;

            curStrm.clear();
            curStrm << curLine;
            unsigned int cptChan=0;
			arma::rowvec chanBuffer(431);
            while( curStrm.good() ) {

                //Node oneNode;

                string value;
                curStrm >> value;
				value = value.substr(0, value.find_first_of(","));
                if( ( value == "NaN" ) ||
                    ( atof(value.c_str()) > MOMAINF && atof( value.c_str() ) >
                    MOMAINF && atof( value.c_str()) > MOMAINF ) ) {
                        // Data are ignored and the oneNode stays with ARMA NaNs
                } else {
                    chanBuffer(cptChan)= atof( value.c_str() );
                    cptChan++;
                }
            }
			positionData.slice(cptFrames).row(X) = chanBuffer.subvec(nodesXId, nodesXId + nbNodes - 1);
			positionData.slice(cptFrames).row(Y) = chanBuffer.subvec(nodesYId, nodesYId + nbNodes - 1);
			positionData.slice(cptFrames).row(Z) = chanBuffer.subvec(nodesZId, nodesZId + nbNodes - 1);
            if (cptFrames==1)
				track->setFrameRate(1.f/chanBuffer(1));
			cptFrames++;

            //            track->push( oneFrame );
        }
    }


    track->position.setData( track->frameRate(), positionData );

    datFile.close();
}

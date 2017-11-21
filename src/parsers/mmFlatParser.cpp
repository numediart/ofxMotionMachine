#include "mmFlatParser.h"

using namespace std;
using namespace MoMa;

FlatParser::FlatParser( std::string const &fileName, MoMa::Track *track ) {

    load( fileName, track );
}

void FlatParser::load( string const &fileName, Track *track ) {

    ifstream datFile( fileName.c_str() ); // Open file

    if( !datFile.is_open() ) {

        cout << "Track: File could not be opened!" << endl;
        return; // We alert on stdout and quit if no file!
    }

    track->clearData();

    unsigned int nbFrames=0,nbNodes=0;
    while ( datFile.good() ) {

        string curLine;
        stringstream curStrm;
        vector<string> rawData;

        getline( datFile, curLine );

        if( curLine != "" && curLine != " " && curLine != "\t" && curLine != "\n" ) {
            curStrm.clear();
            curStrm << curLine;
            if (nbFrames==0){
                nbNodes=std::count( curLine.begin(), curLine.end(), ' ' )/3;
/*                while( curStrm.good() ) {

                    string value[3];
                    curStrm >> value[0];
                    curStrm >> value[1];
                    curStrm >> value[2];
                    nbNodes++;
                }*/
            }
            nbFrames++;
        }
    }
    datFile.clear() ;
    datFile.seekg(0, ios::beg) ;
    
    
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
            unsigned int cptNodes=0;

            while( curStrm.good() ) {

                //Node oneNode;

                string value[3];

                curStrm >> value[0];
                curStrm >> value[1];
                curStrm >> value[2];

                if( ( value[0] == "NaN" && value[1] == "NaN" && value[2] == "NaN" ) ||
                    ( atof(value[0].c_str()) > MOMAINF && atof( value[0].c_str() ) >
                    MOMAINF && atof( value[0].c_str()) > MOMAINF ) ) {

                        // Data are ignored and the oneNode stays with ARMA NaNs

                } else {

                    // oneNode.position(X) = atof( value[0].c_str() );
                    // oneNode.position(Y) = atof( value[1].c_str() );
                    // oneNode.position(Z) = atof( value[2].c_str() );
                    positionData(X,cptNodes,cptFrames)= atof( value[0].c_str() );
                    positionData(Y,cptNodes,cptFrames) = atof( value[1].c_str() );
                    positionData(Z,cptNodes,cptFrames) = atof( value[2].c_str() );
                    cptNodes++;

                }
                if( cptNodes >= nbNodes )
                    break;
            }
            cptFrames++;
            //            track->push( oneFrame );
        }
    }

    track->setFrameRate( 100 );
    cout << "Note: default framerate for a flat file is set at 100 fps. Do not forget to set the right framerate in your track." << endl;
    track->position.setData( track->frameRate(), positionData );

    datFile.close();
}

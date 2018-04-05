#include "mmTsvParser.h"

using namespace std;
using namespace MoMa;

TsvParser::TsvParser(string const &fileName, Track *track) {

    load(fileName, track);
}

void TsvParser::load(string const &fileName, Track *track) {

    track->clearData(); // Clear the track before

    ifstream tsvFile(fileName.c_str());

    if (!tsvFile.is_open()) {

        cout << "Track: File could not be opened!" << endl;
        throw std::runtime_error("File could not be opened.");
        return; // We alert on stdout and quit if no file!
    }

    string thisString;
    //Header
    //Line 1: number of frames
    if (tsvFile.good()) getline(tsvFile, thisLine);//Line 1 (first line)
    thisStream.clear();
    thisStream << thisLine;
    thisStream >> thisString;//Elem 1
    thisStream >> thisString;//Elem 2
    //size_t tab = thisLine.find_last_of("\t");
    //string nframes = thisLine.substr(tab + 1, thisLine.size() - tab - 1);
    unsigned int nbOfFrames = atoi(thisString.c_str());

    //Line 3: numer of markers
    if (tsvFile.good()) getline(tsvFile, thisLine);//Line 2
    if (tsvFile.good()) getline(tsvFile, thisLine);//Line 3
    //tab = thisLine.find_last_of("\t");
    //string nmarkers = thisLine.substr(tab + 1, thisLine.size() - tab - 1);
    thisStream.clear();
    thisStream << thisLine;
    thisStream >> thisString;//Elem 1
    thisStream >> thisString;//Elem 2
    unsigned int nbOfNodes = atoi(thisString.c_str());

    //Line 4: frequency
    if (tsvFile.good()) getline(tsvFile, thisLine);//Line 4
    //tab = thisLine.find_last_of("\t");
    //string freq = thisLine.substr(tab + 1, thisLine.size() - tab - 1);
    thisStream.clear();
    thisStream << thisLine;
    thisStream >> thisString;//Elem 1
    thisStream >> thisString;//Elem 2
    float framerate = atoi(thisString.c_str());

    track->setFrameRate(framerate);


    if (tsvFile.good()) getline(tsvFile, thisLine); // 5
    if (tsvFile.good()) getline(tsvFile, thisLine); // 6
    if (tsvFile.good()) getline(tsvFile, thisLine); // 7
    if (tsvFile.good()) getline(tsvFile, thisLine); // 8
    if (tsvFile.good()) getline(tsvFile, thisLine); // 9
    

    //Line 10: marker names  
    if (tsvFile.good()) getline(tsvFile, thisLine); // 10
    nodeList = std::make_shared<NodeList>(); // We create a nodeList,
    track->nodeList = nodeList; // add it to the track
    track->hasNodeList = true; // and tell everybody  
    track->hasRotation = false;
    //track->position.getRefData().resize(3, nbOfNodes, nbOfFrames);
    arma::cube positionData(3, nbOfNodes, nbOfFrames);
    //Remove unwanted delimiters at the end of the line
    while (thisLine[thisLine.size() - 1] == ' ' || thisLine[thisLine.size() - 1] == '\t' || thisLine[thisLine.size() - 1] == '\n')
        thisLine.resize(thisLine.size() - 1);

    thisStream.clear();
    thisStream << thisLine; // We store the line in a string stream

    string tag; thisStream >> tag;

    while (thisStream.good()) {

        // We save the list of tags
        thisStream >> tag;
        rawJoint.push_back(tag);
    }

    //if (rawJoint[rawJoint.size() - 1] == rawJoint[rawJoint.size() - 2])
    //    rawJoint.resize(rawJoint.size() - 1); //(streamstring bug, it repeats last value if there was a tab at the end of the line)

    for (int r = 0, n = 0; n < nbOfNodes; r += 1, n++) {

        //track->nodeList->at(n) = rawJoint[r];
        track->nodeList->insert(make_pair(rawJoint[r], n));
    }

    //Data

    unsigned int frameCpt = 0; // Init frame count

    int index;

    while (tsvFile.good()) {

        getline(tsvFile, thisLine);

        if (thisLine != "" && thisLine != " " && thisLine != "\t" && thisLine != "\n") {

            arma::mat posMat(3, nbOfNodes);

            thisStream.clear(); // Clear and grab
            thisStream << thisLine; // a new line

            if (thisStream.good()) {

                unsigned int nodeCpt = 0;

                for (int k = 0; k < nbOfNodes; k ++) {

                    string value[12];

                    thisStream >> value[0];
                    thisStream >> value[1];
                    thisStream >> value[2];

                    if (value[0] == "NaN" || atof(value[0].c_str()) > MOMAINF) {

                        // Data are ignored and the matrices
                        // take arma's NaNs as positions/rotations

                        posMat.col(nodeCpt) = arma::ones(3) * arma::datum::nan;
                    }
                    else {

                        posMat(0, nodeCpt) = atof(value[0].c_str());
                        posMat(1, nodeCpt) = atof(value[1].c_str());
                        posMat(2, nodeCpt) = atof(value[2].c_str());

                        /*posMat( axisIndex[k], nodeCpt ) *= 1000;
                        posMat( axisIndex[k+1], nodeCpt ) *= 1000;
                        posMat( axisIndex[k+2], nodeCpt ) *= 1000;*/

                    }
                    nodeCpt++;
                }

                positionData.slice(frameCpt) = posMat; // Put frames at location

                frameCpt++;
            }
        }
    }

    //    track->hasOrigNodeRot_as_boneRot=true;

    //track->position.SetValidParam();
    //if (track->hasRotation)
    //    track->rotation.SetValidParam();

    track->position.setData(framerate, positionData);

    tsvFile.close();
}

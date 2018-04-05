#ifndef __mmTsvParser__
#define __mmTsvParser__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "mmTrack.h"

namespace MoMa {

    class TsvParser {

    public:

        TsvParser(std::string const &fileName, Track *track);
        void load(std::string const &fileName, Track *track);
        void checkemptytabs(std::string &mystring);
    private:

        std::vector<std::string> rawJoint;
        std::vector<int> axisIndex;
        std::shared_ptr<NodeList> nodeList;
        std::shared_ptr<BoneList> boneList;

        std::stringstream thisStream;
        std::string thisLine;
        bool isTimed;
        arma::vec timestamps;
    };
}

#endif

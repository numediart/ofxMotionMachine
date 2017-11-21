#ifndef __mmV3dParser__
#define __mmV3dParser__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "mmTrack.h"
#include "mmQuaternion.h"

namespace MoMa {

    class V3dParser {

      public:

        V3dParser( std::string const &fileName, Track *track );
        void load( std::string const &fileName, Track *track );

      private :

        std::vector<std::string> rawJoint;
        std::vector<int> axisIndex;
        std::shared_ptr<NodeList> nodeList;
        std::shared_ptr<BoneList> boneList;
        int dim;

        std::stringstream thisStream;
        std::string thisLine;
        quaternion quat;
    };
}

#endif

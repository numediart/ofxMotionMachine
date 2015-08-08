#ifndef __mmParser__
#define __mmParser__

#include <string>
#include <vector>

#include "mmTrack.h"
#include "mmQuaternion.h"

#include "mmFlatParser.h"
#include "mmV3dParser.h"
#include "mmC3dParser.h"
#include "mmBvhParser.h"

namespace MoMa {

    class Parser {
        
      public:
        
        Parser( std::string const &fileName, Track *tr );
        
      private:

        std::string checkFileName( std::string const &fileName );
        
        bool setJointOffsetRotation(Track *tr);
        std::string extension;
        std::string fileName;
        
        Track *track;
    };
}

#endif

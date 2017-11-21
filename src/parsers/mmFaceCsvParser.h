#ifndef __mmFaceCsvParser__
#define __mmFaceCsvParser__

#include "mmTrack.h"

namespace MoMa {

    class FaceCsvParser {

      public:

        FaceCsvParser( std::string const &fileName, Track *track );
        void load( std::string const &fileName, Track *track );
    };
}

#endif

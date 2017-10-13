#ifndef __mmFaceTxtParser__
#define __mmFaceTxtParser__

#include "mmTrack.h"

namespace MoMa {

    class FaceTxtParser {

      public:

        FaceTxtParser( std::string const &fileName, Track *track );
        void load( std::string const &fileName, Track *track );
    };
}

#endif

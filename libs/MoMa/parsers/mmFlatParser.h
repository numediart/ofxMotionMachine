#ifndef __mmFlatParser__
#define __mmFlatParser__

#include "mmTrack.h"

namespace MoMa {

    class FlatParser {

      public:

        FlatParser( std::string const &fileName, Track *track );
        void load( std::string const &fileName, Track *track );
    };
}

#endif

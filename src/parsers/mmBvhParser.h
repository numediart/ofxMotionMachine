#ifndef __mmBvhParser__
#define __mmBvhParser__

#include <iostream>

#include "mmTrack.h"

namespace MoMa {
    class BvhParser{
    public:
    
        static bool load( std::string fileName, MoMa::Track *tr, bool hasRotation =true, bool globalCoordinate=false );
    };
}
#endif

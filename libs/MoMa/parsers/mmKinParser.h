#ifndef __mmKinParser__
#define __mmKinParser__

#include "mmTrack.h"

namespace MoMa {

	class KinParser {

	public:

		KinParser( std::string const &fileName, Track *track );
		void load( std::string const &fileName, Track *track );
	};
}

#endif

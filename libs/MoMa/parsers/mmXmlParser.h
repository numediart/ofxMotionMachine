#ifndef __mmXmlParser__
#define __mmXmlParser__

#include "mmTrack.h"

namespace MoMa {

	class XmlParser {

	public:

		XmlParser( std::string const &fileName, Track *track );
		void load( std::string const &fileName, Track *track );
		
	};
}

#endif

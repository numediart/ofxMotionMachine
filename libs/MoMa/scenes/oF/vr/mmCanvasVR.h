


#ifndef __mmCanvasVR__
#define __mmCanvasVR__

#include "ofxOpenVRExtended.h"
#include "ofMain.h"

namespace MoMa {
	class CanvasVR
	{
	public:
		CanvasVR() {};
		virtual ~CanvasVR();

		virtual void setup() {};
		virtual void update() {};
		virtual void draw() {};

	protected:

		ofFbo fbo;
		ofMatrix4x4 fboMatrix;

		bool selected;
	};
}

#endif


#pragma once

#include "ofMain.h"
#include "ofxOpenVRExtended.h"

namespace MoMa {
	class CanvasVR
	{
	public:
		CanvasVR();
		virtual ~CanvasVR();

		virtual void setup() {};
		virtual void update() {};
		virtual void draw() {};

		void attachToController(ControllerRole controller);
		void detachFromController();

	protected:
		ofFbo fbo;
		ofMatrix4x4 fboMatrix;

		bool attachedToController;
		ControllerRole attachedTo;

		bool selected;
	};
}


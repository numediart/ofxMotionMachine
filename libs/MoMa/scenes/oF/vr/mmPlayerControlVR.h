

#ifndef __mmPlayerControlVR__
#define __mmPlayerControlVR__

namespace MoMa {
	class PlayerControlVR;
}


#include "mmCanvasVR.h"
#include "mmUtils.h"
#include "mmSceneVR.h"

namespace MoMa {
	enum VisualIdentifier
	{
		PAUSE_000 = 0,
		PAUSE_001,
		PAUSE_002,
		PAUSE_010,
		PAUSE_020,
		PAUSE_100,
		PAUSE_200,

		PLAY_000,
		PLAY_001,
		PLAY_002,
		PLAY_010,
		PLAY_020,
		PLAY_100,
		PLAY_200,

		SCRUB_0,
		SCRUB_1,

		STOP_2
	};

	class PlayerControlVR :
		public CanvasVR
	{
	public:
		PlayerControlVR();
		PlayerControlVR(SceneVR *_app, ControllerState *controller);
		virtual ~PlayerControlVR();

		void attachToController(ControllerState *controller);
		void controllerEvent(ofxOpenVRControllerEventArgs& args);

		void setup();
		void update();
		void draw();

	protected:

		SceneVR *app;
		bool attachedToController;
		ControllerState *attachedTo;
		vr::ETrackedControllerRole controllerRole;

		vector<ofImage> visuals;
		VisualIdentifier currentVisual;
	};
}

#endif
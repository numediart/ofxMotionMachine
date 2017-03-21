



#ifndef __mmSceneVR__
#define __mmSceneVR__

namespace MoMa {
	class SceneVR;
}

#include "mmSceneApp.h"
#include "ofMain.h"
#include "ofxOpenVRExtended.h"

#include "MoMa.h"
#include "MoMaUI.h"
#include "ofMain.h"
#include "mmOfSkeleton.h"

//#include "mmCanvasVR.h"
#include "mmPlayerControlVR.h"



namespace MoMa {

	class SceneVR : public  MoMa::SceneApp {
	protected:
		typedef shared_ptr<MoMa::Track> trackPtr;
		typedef shared_ptr<MoMa::ofSkeleton> ofSkeletonPtr;
	public:
		SceneVR() {
			mouseX = mouseY = 0;
		};



		void render(vr::Hmd_Eye nEye);
		void controllerEvent(ofxOpenVRControllerEventArgs& args);
		virtual void setup() {};
		virtual void update() {};
		virtual void exit() {};
		virtual void scene2d() {};
		virtual void scene3d() {};
		virtual void draw3d() {};//draw an 3D element not linked to the MoMaScene

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxOpenVRExtended openVR;

		bool bShowHelp, debugInfoFlag;
		std::ostringstream _strHelp;

		ofImage image;
		//ofShader shader;
		ofLight light, light2;
		ofLight iconLight;
		ofEasyCam cam;
		ofFbo fbo;
	protected:

		virtual void setup(ofEventArgs &args);
		virtual void update(ofEventArgs &args);
		virtual void draw(ofEventArgs &args);
		virtual void exit(ofEventArgs &args);


		PlayerControlVR *playerControl;

		ControllerState controllers[2];
		ofMatrix4x4 controllersMatrix[2];
		ofMatrix4x4 sceneMat,initSceneMat;
		float sceneScale;
		ofMatrix4x4 floatingSceneMat;
		float floatingSceneScale;
		bool floatingSceneFlag;
		int floatingTranslSceneFlag;
		bool Scene2DDisplayFlag;
		int scrubControllerIndex;
	};

};

#endif
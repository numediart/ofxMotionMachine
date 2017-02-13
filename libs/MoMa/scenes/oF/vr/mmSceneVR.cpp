
#include "mmSceneVR.h"

// Thanks to @num3ric for sharing this:
// http://discourse.libcinder.org/t/360-vr-video-player-for-ios-in-cinder/294/6


using namespace MoMa;
#define NoHMD
//--------------------------------------------------------------
void SceneVR::setup(ofEventArgs &args) {

	ofSetVerticalSync(true);
	ofDisableArbTex();

	// We need to pass the method we want ofxOpenVR to call when rending the scene
#ifndef NoHMD
	openVR.setup(std::bind(&SceneVR::render, this, std::placeholders::_1));

	openVR.setRenderModelForTrackedDevices(true);
	
	controllers[0].role = ControllerRole::Right;
	controllers[1].role = ControllerRole::Left;
	playerControl = new PlayerControlVR(this, &controllers[0]);
	// controller event callback
	ofAddListener(openVR.ofxOpenVRControllerEvent, this, &SceneVR::controllerEvent);
	ofAddListener(openVR.ofxOpenVRControllerEvent, playerControl, &PlayerControlVR::controllerEvent);

#else
	cam.setGlobalPosition(-5, 1, 0);
	cam.lookAt(ofVec3f(0, 1, 0), ofVec3f(0, 1, 0));
	cam.setNearClip(0.1);
	Scene2DDisplayFlag = true;
	//	cam.setFarClip(3000);
#endif



	ofSetSmoothLighting(true);
	light.setDiffuseColor(ofFloatColor(.85, .85, .55));
	light.setSpecularColor(ofFloatColor(1.f, 1.f, 1.f));
	light.setPosition(2, 2, -2);
	light2.setDiffuseColor(ofFloatColor(.85, .85, .55));
	light2.setSpecularColor(ofFloatColor(1.f, 1.f, 1.f));
	light2.setPosition(-2, 2, 2);
	bShowHelp = true;
	floatingSceneFlag = false;
	floatingTranslSceneFlag = -1;
	sceneMat.makeIdentityMatrix();
	sceneScale = 1;
	floatingSceneScale = 1;

	fbo.allocate(ofGetWidth(), ofGetHeight());
	Scene2DDisplayFlag = false;
	SceneApp::setup(args);
	ofSetFrameRate(90);
#ifdef NoHMD
	Scene2DDisplayFlag = true;
	ofSetFrameRate(1000);
#endif
}

//--------------------------------------------------------------
void SceneVR::exit(ofEventArgs &args) {
	delete playerControl;
	SceneApp::exit(args);
#ifndef NoHMD
	openVR.exit();
#endif
}

//--------------------------------------------------------------
void SceneVR::update(ofEventArgs &args) {
#ifndef NoHMD
	openVR.update();
	playerControl->update();

	if (floatingTranslSceneFlag >= 0) {

		ofMatrix4x4 curLeftHand = &openVR.getControllerPose(vr::TrackedControllerRole_LeftHand)[0][0];
		ofMatrix4x4 curRightHand = &openVR.getControllerPose(vr::TrackedControllerRole_RightHand)[0][0];

		ofVec3f translValue;
		//controllersMatrix[0] = &openVR.getControllerPose(vr::TrackedControllerRole_LeftHand)[0][0];
		//controllersMatrix[1] = &openVR.getControllerPose(vr::TrackedControllerRole_RightHand)[0][0];
		if (floatingSceneFlag)
			translValue = (curLeftHand.getRowAsVec3f(3) + curRightHand.getRowAsVec3f(3) - controllersMatrix[0].getRowAsVec3f(3) - controllersMatrix[1].getRowAsVec3f(3)) / 2;
		else
			if (floatingTranslSceneFlag == 0)
				translValue = curLeftHand.getRowAsVec3f(3) - controllersMatrix[0].getRowAsVec3f(3);
			else
				translValue = curRightHand.getRowAsVec3f(3) - controllersMatrix[1].getRowAsVec3f(3);

		ofVec3f dest = curLeftHand.getRowAsVec3f(3) - curRightHand.getRowAsVec3f(3);
		dest.y = 0;
		ofVec3f orig = controllersMatrix[0].getRowAsVec3f(3) - controllersMatrix[1].getRowAsVec3f(3);
		orig.y = 0;
		floatingSceneMat.makeIdentityMatrix();
		floatingSceneMat.translate(translValue);

		if (floatingSceneFlag) {
			ofVec3f pivotValue = (curLeftHand.getRowAsVec3f(3) + curRightHand.getRowAsVec3f(3)) / 2;
			float rotValue = acos(orig.normalized().dot(dest.normalized()));
			if (orig.getCrossed(dest).y < 0)
				rotValue = (-rotValue);
			floatingSceneScale = sqrt(dest.dot(dest) / orig.dot(orig));
			floatingSceneMat.translate(-pivotValue);
			floatingSceneMat.scale(floatingSceneScale*ofVec3f(1, 1, 1));
			floatingSceneMat.rotate(ofQuaternion(RAD_TO_DEG* rotValue, ofVec3f(0, 1, 0)));
			floatingSceneMat.translate(pivotValue);
		}
		//std::cout << "rot:"<<rotValue << " scale:" << floatingSceneScale << " trans:" << translValue << std::endl;


	}
#endif
	//ofEnableDepthTest();
	if (Scene2DDisplayFlag) {
		this->is2D = true;
		fbo.begin();
		ofClear(225, 225, 225);
		ofSetColor(200, 200, 200);
		ofRect(0, 0, ofGetWidth(), ofGetHeight());

		scene2d(); // 2D figures
		render2d();
		fbo.end();
		this->is2D = false;
	}

	//ofEnableDepthTest();
	SceneApp::update(args);
}


void SceneVR::draw(ofEventArgs &args) {
#ifndef NoHMD
	openVR.render();
	openVR.renderDistortion();

	openVR.drawDebugInfo(10.0f, 500.0f);

	// Help
	if (bShowHelp) {
		_strHelp.str("");
		_strHelp.clear();
		_strHelp << "HELP (press h to toggle): " << endl;
		_strHelp << "Drag and drop a 360 spherical (equirectangular) image to load it in the player. " << endl;
		_strHelp << "Toggle OpenVR mirror window (press: m)." << endl;
		ofDrawBitmapStringHighlight(_strHelp.str(), ofPoint(10.0f, 20.0f), ofColor(ofColor::black, 100.0f));
	}

#else
	ofPushView();

	ofDrawBitmapString("frame rate:"+ ofToString(ofGetFrameRate())+" fps", 10,10);
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	//ofLoadMatrix(openVR.getCurrentProjectionofMatrix(nEye));
	ofLoadMatrix(cam.getProjectionMatrix());
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	//ofMatrix4x4 currentViewMatrixInvertY = openVR.getCurrentViewofMatrix(nEye);
	ofMatrix4x4 currentViewMatrixInvertY = cam.getModelViewMatrix();
	currentViewMatrixInvertY.scale(1.0f, -1.0f, 1.0f);
	ofLoadViewMatrix(currentViewMatrixInvertY);

	ofEnableDepthTest();
	ofEnableLighting();
	light.enable();
	light2.enable();
	cam.begin();
	cam.end();
	ofSetBackgroundColor(100);


	ofDrawGrid(0.25, 10, false, false, true, false);

	if (Scene2DDisplayFlag) {
		ofPushMatrix();
		ofTranslate(0.5, 1.5, -ofGetWidth() / 2000.f);
		ofRotateX(180);
		ofRotateY(90);
		ofTranslate(0,0,-2);
		fbo.draw(0, 0, ofGetWidth() / 1000.f, ofGetHeight() / 1000.f);
		ofPopMatrix();
	}
	ofPushMatrix();
	ofRotateY(180);

	ofRotateX(-90);
	ofScale(0.001, 0.001, 0.001);

	scene3d();

	ofPopMatrix();
	ofPushMatrix();
	ofTranslate(0, 1, 0);

	ofDrawAxis(1.0);
	ofPopMatrix();
	draw3d();
	light.disable();
	light2.disable();
	ofDisableLighting();
	ofDisableDepthTest();
	ofPopView();
#endif
}

//--------------------------------------------------------------
void  SceneVR::render(vr::Hmd_Eye nEye) {

	ofPushView();
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(&openVR.getCurrentProjectionMatrix(nEye)[0][0]);

	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofMatrix4x4 currentViewMatrixInvertY = &openVR.getCurrentViewMatrix(nEye)[0][0];
	currentViewMatrixInvertY.scale(1.0f, -1.0f, 1.0f);
	ofLoadViewMatrix(currentViewMatrixInvertY);



	ofEnableLighting();
	light.enable();
	light2.enable();

	ofEnableDepthTest();

	if (Scene2DDisplayFlag) {
		ofPushMatrix();
		ofTranslate(0.5, 1.5, -ofGetWidth() / 2000.f);
		ofRotateX(180);
		ofRotateY(90);
		fbo.draw(0, 0, ofGetWidth() / 1000.f, ofGetHeight() / 1000.f);
		ofPopMatrix();
	}

	ofSetBackgroundColor(20);

	ofSetColor(150);
	ofPushMatrix();
	ofRotateX(90);
	ofDrawPlane(5, 5);
	ofPopMatrix();
	ofPushMatrix();
	//ofMultMatrix(floatingSceneMat);
	ofMatrix4x4 appliedMatrix = sceneMat*floatingSceneMat;

	appliedMatrix(3, 1) = appliedMatrix(3, 1)< 0 ? 0 : appliedMatrix(3, 1);//ground limit
	ofMultMatrix(appliedMatrix);

	ofPushMatrix();
	ofRotateZ(90);
	ofPushStyle();
	ofSetColor(50);
	ofDrawGridPlane(.25, 10);
	ofPopStyle();
	ofPopMatrix();
	//ofDrawGrid(0.25, 10, false, false, true, false);
	ofPushMatrix();
	//ofTranslate(0, 0, -1);
	ofRotateY(-90);
	ofPushMatrix();
	ofRotateX(-90);
	ofScale(0.001, 0.001, 0.001);
	ofPushStyle();
	scene3d();
	ofPopStyle();
	ofPopMatrix();

	ofPopMatrix();
	ofPopMatrix();
	draw3d();
	playerControl->draw();

	light.disable();
	light2.disable();
	ofDisableLighting();
	ofPopView();

}

//--------------------------------------------------------------
void SceneVR::controllerEvent(ofxOpenVRControllerEventArgs& args) {
	/*	cout << "SceneVR::controllerEvent > role: " << int(args.controllerRole);
	cout << " - event type: " << int(args.eventType);
	cout << " - button type: " << int(args.buttonType);
	cout << " - x: " << args.analogInput_xAxis << " - y: " << args.analogInput_yAxis << endl;
	*/
	if (args.controllerRole < ControllerRole::Unknown) {
		int controllerId = (int)(args.controllerRole);
		// Trigger
		if (args.buttonType == ButtonType::ButtonTrigger) {
			if (args.eventType == EventType::ButtonPress) {
				controllers[controllerId].triggerPressed = true;
				controllers[controllerId].triggerPressedStartTime = ofGetElapsedTimef();
				// check if other trigger is already pressed
				int otherController = controllerId == 0 ? 1 : 0;
				if (controllers[otherController].triggerPressed) {
					sceneMat = sceneMat*floatingSceneMat;
					sceneMat(3, 1) = sceneMat(3, 1)< 0 ? 0 : sceneMat(3, 1);
					floatingSceneMat.makeIdentityMatrix();
					floatingSceneFlag = true;
					// save both position / rotation matrix
					//cout << "left matrix : " << controllersMatrix[0] << endl;
					//cout << "right matrix : " << controllersMatrix[1] << endl;
				}
				controllersMatrix[0] = &openVR.getControllerPose(vr::TrackedControllerRole_LeftHand)[0][0];
				controllersMatrix[1] = &openVR.getControllerPose(vr::TrackedControllerRole_RightHand)[0][0];

				floatingTranslSceneFlag = controllerId;


			}
			else if (args.eventType == EventType::ButtonUnpress) {
				sceneMat = sceneMat*floatingSceneMat;
				sceneMat(3, 1) = sceneMat(3, 1)< 0 ? 0 : sceneMat(3, 1);
				floatingSceneMat.makeIdentityMatrix();
				//if (floatingSceneFlag) {
				//floatingSceneScale = 1;
				//controllers[0].triggerPressedStartTime = ofGetElapsedTimef();
				//controllers[1].triggerPressedStartTime = ofGetElapsedTimef();
				//				}

				controllers[controllerId].triggerPressedStartTime = ofGetElapsedTimef();

				floatingSceneFlag = false;

				floatingTranslSceneFlag = -1;
				controllers[controllerId].triggerPressed = false;
				float timePressed = ofGetElapsedTimef() - controllers[controllerId].triggerPressedStartTime;
				//cout << "controller " << controllerId << ", trigger pressed for " << timePressed << "seconds" << endl;
			}
		}

		// Menu
		else if (args.buttonType == ButtonType::ButtonApplicationMenu) {
			if (args.eventType == EventType::ButtonPress) {
				controllers[controllerId].menuPressed = true;
				controllers[controllerId].menuPressedStartTime = ofGetElapsedTimef();
			}
			else if (args.eventType == EventType::ButtonUnpress) {

				if (ofGetElapsedTimef() - controllers[controllerId].menuPressedStartTime  > 1) {
					sceneMat.makeIdentityMatrix();
					floatingSceneMat.makeIdentityMatrix();
					sceneScale = 1;
					floatingSceneScale = 1;
				}
				else {
					Scene2DDisplayFlag = Scene2DDisplayFlag ? false : true;
				}

				controllers[controllerId].menuPressed = false;
				float timePressed = ofGetElapsedTimef() - controllers[controllerId].menuPressedStartTime;
				//cout << "controller " << controllerId << ", menu pressed for " << timePressed << "seconds" << endl;
			}
		}
	}
}

//--------------------------------------------------------------
void SceneVR::keyPressed(int key) {
}

//--------------------------------------------------------------
void SceneVR::keyReleased(int key) {

}

//--------------------------------------------------------------
void SceneVR::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void SceneVR::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void SceneVR::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void SceneVR::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void SceneVR::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void SceneVR::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void SceneVR::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void SceneVR::gotMessage(ofMessage msg) {

}



//--------------------------------------------------------------
void SceneVR::dragEvent(ofDragInfo dragInfo) {
}
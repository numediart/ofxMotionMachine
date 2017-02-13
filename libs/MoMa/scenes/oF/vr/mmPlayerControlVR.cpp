#include "mmPlayerControlVR.h"

namespace MoMa {

	PlayerControlVR::PlayerControlVR()
	{
	}

	PlayerControlVR::PlayerControlVR(SceneVR *_app, ControllerState *controller) : app(_app)
	{
		string libPath = getAbsoluteResPath();
		string fileNames[] = { "pause_000.png",
								"pause_001.png",
								"pause_002.png",
								"pause_010.png",
								"pause_020.png",
								"pause_100.png",
								"pause_200.png",
								"play_000.png",
								"play_001.png",
								"play_002.png",
								"play_010.png",
								"play_020.png",
								"play_100.png",
								"play_200.png",
								"scrub_0.png",
								"scrub_1.png",
								"stop_2.png"};
		for (string f : fileNames)
		{
			visuals.push_back(ofImage(libPath + "GUIVR/" + f));
		}

		attachToController(controller);
		setup();
	}

	PlayerControlVR::~PlayerControlVR()
	{

	}

	void PlayerControlVR::attachToController(ControllerState *controller)
	{
		attachedToController = true;
		attachedTo = controller;

		vr::VRControllerState_t pControllerState;
		switch (controller->role)
		{
		case ControllerRole::Left:
			controllerRole = vr::TrackedControllerRole_LeftHand;
			break;
		case ControllerRole::Right:
			controllerRole = vr::TrackedControllerRole_RightHand;
			break;
		default:
			break;
		}
	}


	void PlayerControlVR::setup()
	{
		fbo.allocate(visuals[0].getWidth(), visuals[0].getHeight());
		currentVisual = VisualIdentifier::PLAY_000;

		fboMatrix.rotate(274.8, 1.0, 0.0, 0.0);
		fboMatrix.translate(-0.023, 0.009, 0.070);

	}

	void PlayerControlVR::update()
	{
		if (app->playbackMode == MoMa::SCRUB && attachedTo->touchpadTouched) {
			app->openVR.updateControllerAnalogData(*attachedTo);
			app->appMoment.setTime(ofMap(attachedTo->touchpadCoordLast.x, -0.8, 0.8,
				app->lowBound.time(), app->highBound.time(), true), app->frameRate);
		}

		if (app->playbackMode == MoMa::PLAY && attachedTo->touchpadTouched) {
			currentVisual = VisualIdentifier::PLAY_010;
		}
		
		fbo.begin();
		visuals[currentVisual].draw(0, 0);
		fbo.end();
	}

	void PlayerControlVR::draw()
	{
		if (app->openVR.isControllerConnected(controllerRole)) {
			ofMatrix4x4 locMat = &(app->openVR).getControllerPose(controllerRole)[0][0];
			ofPushMatrix();
			ofMultMatrix(locMat);
			ofMultMatrix(fboMatrix);
			//ofPushStyle();
			fbo.draw(0, 0, 0.046, 0.046);
			//ofPopStyle();
			ofPopMatrix();
		}
	}

	void PlayerControlVR::controllerEvent(ofxOpenVRControllerEventArgs& args)
	{
		if (args.controllerRole == attachedTo->role)
		{
			// Button Touchpad
			if (args.buttonType == ButtonType::ButtonTouchpad) {
				if (args.eventType == EventType::ButtonPress) {
					attachedTo->touchpadPressed = true;
					attachedTo->touchpadPressedStartTime = ofGetElapsedTimef();
					attachedTo->touchpadCoordOrg.x = args.analogInput_xAxis;
					attachedTo->touchpadCoordOrg.y = args.analogInput_yAxis;
				}
				else if (args.eventType == EventType::ButtonUnpress) {
					attachedTo->touchpadPressed = false;
					float timePressed = ofGetElapsedTimef() - attachedTo->touchpadPressedStartTime;

					if (app->playbackMode == MoMa::PLAY) {
						float xcoord = attachedTo->touchpadCoordOrg.x;
						if (timePressed < 0.6) {
							// short press on the center of the touchpad -> play / pause
							if (xcoord > -0.5 && xcoord < 0.5) {
								if (app->isPlaying()) {
									app->pause();
								}
								else {
									app->play();
								}
							}
							else {
								// short press on the left / right border of the pad -> prev / next
								double targetTime = app->appMoment.time();
								if (xcoord > 0.75) {
									app->pause();
									targetTime += 0.05;
									if (targetTime > app->highBound.time()) targetTime = app->highBound.time();
									app->appMoment.setTime(targetTime);
									//cout << "next" << endl;
								}
								else if (xcoord < -0.75) {
									app->pause();
									targetTime -= 0.05;
									if (targetTime < app->lowBound.time()) targetTime = app->lowBound.time();
									app->appMoment.setTime(targetTime);
									//cout << "previous" << endl;
								}

							}
						}
						// long press on the center -> stop
						else if (xcoord > -0.5 && xcoord < 0.5) {
							app->stop();
						}
					}
				}
				else if (args.eventType == EventType::ButtonTouch) {
					attachedTo->touchpadTouched = true;
					attachedTo->touchpadTouchedStartTime = ofGetElapsedTimef();
					attachedTo->touchpadCoordOrg.x = args.analogInput_xAxis;
					attachedTo->touchpadCoordOrg.y = args.analogInput_yAxis;
					//cout << "touch init at " << controllers[controllerId].touchpadCoordOrg << endl;
				}
				else if (args.eventType == EventType::ButtonUntouch) {
					attachedTo->touchpadTouched = false;
					float timePressed = ofGetElapsedTimef() - attachedTo->touchpadTouchedStartTime;
					ofVec2f delta = attachedTo->touchpadCoordLast - attachedTo->touchpadCoordOrg;
					//cout << "controller " << controllerId << ", touchpad touched for " << timePressed
					//	<< "seconds with a deplacement of " << delta << endl;
				}
			}
			// Grip
			else if (args.buttonType == ButtonType::ButtonGrip) {
				if (args.eventType == EventType::ButtonPress) {
					attachedTo->gripPressed = true;
					attachedTo->gripPressedStartTime = ofGetElapsedTimef();
				}
				else if (args.eventType == EventType::ButtonUnpress) {
					attachedTo->gripPressed = false;
					float timePressed = ofGetElapsedTimef() - attachedTo->gripPressedStartTime;
					//cout << "controller " << controllerId << ", grip pressed for " << timePressed << "seconds" << endl;
					app->pause();
					if (app->playbackMode == MoMa::SCRUB) {
						app->setPlaybackMode(MoMa::PLAY);
					}
					else {
						app->pause();
						app->setPlaybackMode(MoMa::SCRUB);
					}
				}
			}
		}
	}
}
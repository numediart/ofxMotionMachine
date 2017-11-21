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

		fboMatrix.rotate(94.8, 1.0, 0.0, 0.0);
		fboMatrix.translate(-0.023, 0.015, 0.024);

	}

	void PlayerControlVR::update()
	{
		// just touched -> highlight touched icon on visual
		if (attachedTo->touchpadTouched) {
			app->openVR.updateControllerAnalogData(*attachedTo);
			float xcoord = attachedTo->touchpadCoordLast.x;
			if (app->playbackMode == MoMa::PLAY) {
				if (xcoord > -0.5 && xcoord < 0.5) {
					if (app->isPlaying())
						currentVisual = VisualIdentifier::PAUSE_010;
					else
						currentVisual = VisualIdentifier::PLAY_010;
				}
				else {
					if (xcoord > 0.75) {
						if (app->isPlaying())
							currentVisual = VisualIdentifier::PAUSE_001;
						else
							currentVisual = VisualIdentifier::PLAY_001;
					}
					else if (xcoord < -0.75) {
						if (app->isPlaying())
							currentVisual = VisualIdentifier::PAUSE_100;
						else
							currentVisual = VisualIdentifier::PLAY_100;
					}
				}
			}
			else if (app->playbackMode == MoMa::SCRUB) {
				// update scrub value at the same time
				app->appMoment.setTime(ofMap(xcoord, -0.8, 0.8,
					app->lowBound.time(), app->highBound.time(), true), app->frameRate);
				currentVisual = VisualIdentifier::SCRUB_1;
			}
		}
		// long press
		float timePressed = ofGetElapsedTimef() - attachedTo->touchpadPressedStartTime;
		if (app->playbackMode == MoMa::PLAY
			&& attachedTo->touchpadPressed
			&& timePressed > longPressTime) {
			// in the center -> visual = stop
			if (attachedTo->touchpadCoordOrg.x > -0.5
				&& attachedTo->touchpadCoordOrg.x < 0.5) {
				currentVisual = VisualIdentifier::STOP_2;
			}
			// on the right -> next frame (quick)
			double targetTime = app->appMoment.time();
			float momentIncrement = (timePressed - longPressTime) * 0.05;
			if (momentIncrement > 0.2) momentIncrement = 0.2;
			if (attachedTo->touchpadCoordOrg.x > 0.75) {
				app->pause();
				targetTime += momentIncrement;
				if (targetTime > app->highBound.time()) targetTime = app->highBound.time();
				app->appMoment.setTime(targetTime);
			}
			// on the left -> previous frame (quick)
			else if (attachedTo->touchpadCoordOrg.x < -0.75) {
				app->pause();
				targetTime -= momentIncrement;
				if (targetTime < app->lowBound.time()) targetTime = app->lowBound.time();
				app->appMoment.setTime(targetTime);
			}
			
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
					
					float xcoord = args.analogInput_xAxis;
					if (app->playbackMode == MoMa::PLAY) {
						if (xcoord > -0.5 && xcoord < 0.5) {
							if (app->isPlaying())
								currentVisual = VisualIdentifier::PAUSE_020;
							else
								currentVisual = VisualIdentifier::PLAY_020;
						}
						else {
							if (xcoord > 0.75) {
								if (app->isPlaying())
									currentVisual = VisualIdentifier::PAUSE_002;
								else
									currentVisual = VisualIdentifier::PLAY_002;
							}
							else if (xcoord < -0.75) {
								if (app->isPlaying())
									currentVisual = VisualIdentifier::PAUSE_200;
								else
									currentVisual = VisualIdentifier::PLAY_200;
							}
						}
					}
				}
				else if (args.eventType == EventType::ButtonUnpress) {
					attachedTo->touchpadPressed = false;
					float timePressed = ofGetElapsedTimef() - attachedTo->touchpadPressedStartTime;

					if (app->playbackMode == MoMa::PLAY) {
						float xcoord = attachedTo->touchpadCoordOrg.x;
						if (timePressed < longPressTime) {
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
						// update visual
						if (app->isPlaying())
							currentVisual = VisualIdentifier::PAUSE_000;
						else
							currentVisual = VisualIdentifier::PLAY_000;
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
					//float timePressed = ofGetElapsedTimef() - attachedTo->touchpadTouchedStartTime;
					//ofVec2f delta = attachedTo->touchpadCoordLast - attachedTo->touchpadCoordOrg;
					//cout << "controller " << controllerId << ", touchpad touched for " << timePressed
					//	<< "seconds with a deplacement of " << delta << endl;
					if (app->playbackMode == MoMa::PLAY) {
						if (app->isPlaying())
							currentVisual = VisualIdentifier::PAUSE_000;
						else
							currentVisual = VisualIdentifier::PLAY_000;
					}
					else if (app->playbackMode == MoMa::SCRUB) {
						currentVisual = VisualIdentifier::SCRUB_0;
					}
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
						currentVisual = VisualIdentifier::PLAY_000;
					}
					else {
						app->pause();
						app->setPlaybackMode(MoMa::SCRUB);
						currentVisual = VisualIdentifier::SCRUB_0;
					}
				}
			}
		}
	}
}
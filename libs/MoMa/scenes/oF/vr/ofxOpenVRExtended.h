#pragma once
#include "ofxOpenVR.h"


struct ControllerState {
	ControllerState::ControllerState() :
		triggerPressed(false),
		touchpadPressed(false),
		touchpadTouched(false),
		menuPressed(false),
		gripPressed(false) {};

	ControllerRole role;
	bool triggerPressed;
	float triggerValue;
	float triggerPressedStartTime;
	bool touchpadPressed;
	float touchpadPressedStartTime;
	bool touchpadTouched;
	float touchpadTouchedStartTime;
	ofVec2f touchpadCoordOrg;
	ofVec2f touchpadCoordLast;
	bool gripPressed;
	float gripPressedStartTime;
	bool menuPressed;
	float menuPressedStartTime;
};

class ofxOpenVRExtended :
	public ofxOpenVR
{
public:
	virtual ~ofxOpenVRExtended();

	void updateControllerAnalogData(ControllerState &controller);
};


#include "ofxOpenVRExtended.h"


ofxOpenVRExtended::~ofxOpenVRExtended()
{
}

//--------------------------------------------------------------
// Purpose: update coord on touchpad anytime if a new value is available
//--------------------------------------------------------------
void ofxOpenVRExtended::updateControllerAnalogData(ControllerState &controller)
{
	vr::VRControllerState_t pControllerState;
	switch (controller.role)
	{
	case ControllerRole::Left:
		vr::VRSystem()->GetControllerState(vr::TrackedControllerRole_LeftHand, &pControllerState, sizeof(pControllerState));
		break;
	case ControllerRole::Right:
		vr::VRSystem()->GetControllerState(vr::TrackedControllerRole_RightHand, &pControllerState, sizeof(pControllerState));
		break;
	default:
		break;
	}
	if (pControllerState.ulButtonTouched == ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) {
		controller.touchpadCoordLast = ofVec2f(pControllerState.rAxis->x, pControllerState.rAxis->y);
		cout << controller.touchpadCoordLast << endl;
	}
	/*if (pControllerState.ulButtonTouched == ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) {
		controller.triggerValue = pControllerState.rAxis[1].x;
	}*/
}

#include "ofxOpenVRExtended.h"


ofxOpenVRExtended::~ofxOpenVRExtended()
{
}

//--------------------------------------------------------------
// Purpose: update coord on touchpad anytime if a new value is available
//--------------------------------------------------------------
void ofxOpenVRExtended::updateControllerAnalogData(ControllerState &controller)
{
	// find device ID for this crontroller
	vr::ETrackedControllerRole vrRole = vr::TrackedControllerRole_Invalid;
	if (controller.role == ControllerRole::Left)
	{
		vrRole = vr::TrackedControllerRole_LeftHand;
	}
	if (controller.role == ControllerRole::Right)
	{
		vrRole = vr::TrackedControllerRole_RightHand;
	}
	int nDevice = 0;
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if (_pHMD->GetControllerRoleForTrackedDeviceIndex(i) == vrRole)
		{
			nDevice = i;
		}
	}
	// update data
	vr::VRControllerState_t pControllerState;
	vr::VRSystem()->GetControllerState(nDevice, &pControllerState, sizeof(pControllerState));
	if (pControllerState.ulButtonTouched == ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) {
		controller.touchpadCoordLast = ofVec2f(pControllerState.rAxis->x, pControllerState.rAxis->y);
	}
	/*if (pControllerState.ulButtonTouched == ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) {
		controller.triggerValue = pControllerState.rAxis[1].x;
	}*/
}

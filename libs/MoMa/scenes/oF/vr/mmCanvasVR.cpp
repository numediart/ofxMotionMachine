#include "mmCanvasVR.h"


using namespace MoMa;

CanvasVR::CanvasVR()
{
}


CanvasVR::~CanvasVR()
{
}

void CanvasVR::attachToController(ControllerRole controller)
{
	if (controller == ControllerRole::Left || controller == ControllerRole::Right)
	{
		attachedToController = true;
		attachedTo = controller;
	}
}

void CanvasVR::detachFromController()
{
	attachedToController = false;
	attachedTo = ControllerRole::Unknown;
}

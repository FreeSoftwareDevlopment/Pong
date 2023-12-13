#include "pch.h"
#include "framework.h"
#include "controllerLibrary.h"
#include <memory>
#include "conf.h"
#include <math.h>

CcontrollerLibrary::CcontrollerLibrary(unsigned long controllerID)
{
	this->controllerID = controllerID;

	pr = malloc(sizeof(XINPUT_STATE));
	if (pr != nullptr) {
		ZeroMemory(pr, sizeof(XINPUT_STATE));
	}
	ZeroMemory(&gp, sizeof(Gamepad));
}

bool CcontrollerLibrary::isReady()
{
	return pr != nullptr;
}

void CcontrollerLibrary::vibrateActiveController(unsigned long L, unsigned long R, bool c){
	if (cached&&c) {
		if (vibCacheL == L && vibCacheR == R) return;
	}

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = L; // use any value between 0-65535 here 
	vibration.wRightMotorSpeed = R; // use any value between 0-65535 here
	XInputSetState(controllerID, &vibration);

	vibCacheR = R;
	vibCacheL = L;
	cached = true;
}

float callexTrigger(const SHORT& triggerX, const SHORT& triggerY, const int INPUT_DEADZONE, COORDS* c) {
	float LX = triggerX;
	float LY = triggerY;

	//determine how far the controller is pushed
	float magnitude = sqrt(LX * LX + LY * LY);

	//determine the direction the controller is pushed
	c->x = LX / magnitude;
	c->y = LY / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > INPUT_DEADZONE)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= INPUT_DEADZONE;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}
	return normalizedMagnitude;
}

#define c(x) static_cast<XINPUT_STATE*>(x)
#define c1(x) c(x)->Gamepad
const Gamepad* CcontrollerLibrary::update()
{
	if (!isReady()) return nullptr;

	if (XInputGetState(controllerID, c(pr)) == ERROR_SUCCESS)
	{
		gp.buttons = c1(pr).wButtons;
		gp.rightTrigger = c1(pr).bRightTrigger;
		gp.leftTrigger = c1(pr).bLeftTrigger;
		gp.leftMagnitude = callexTrigger(c1(pr).sThumbLX, c1(pr).sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, &gp.nL);
		gp.rightMagnitude = callexTrigger(c1(pr).sThumbRX, c1(pr).sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, &gp.nR);
		return &gp;
	}
	return nullptr;
}
#undef c1
#undef c

CcontrollerLibrary::~CcontrollerLibrary()
{
	if (pr != nullptr)
		free(pr);

	vibrateActiveController(0, 0, false); // Stop Vibration
}

#include "Input.h"

namespace seq {
	void Input::Init()
	{
		for (auto i = 0ui8; i < 255ui8; i++) { buttonState[i] = 0; buttonCheck[i] = 0; }
		connectedPads = JslConnectDevices();
		chosenPad = 0;
		if (connectedPads < 1) return;
		if (connectedPads > 4) connectedPads = 4;
		JslGetConnectedDeviceHandles(handledPads, connectedPads);
	}
	void Input::Tick()
	{
		bool b = ((GetAsyncKeyState(0) & 0x8000) >> 15);
		for (auto i = 0ui8; i < 255ui8; i++)
		{
			b = ((GetAsyncKeyState(i) & 0x8000) >> 15);
			buttonCheck[i] = (b == buttonState[i]) * 1;
			buttonState[i] = b;
		}
	}
	void Input::Destroy()
	{

	}
	const bool Input::GetKey(ButtonKey key)
	{
		return buttonState[key];
	}
	const bool Input::GetKeyDown(ButtonKey key)
	{
		return buttonState[key] && buttonCheck[key];
	}
	const bool Input::GetKeyUp(ButtonKey key)
	{
		return !buttonState[key] && buttonCheck[key];
	}
	const void Input::GetLeftXY(int& x, int& y)
	{
		JslGetLeftX(handledPads[chosenPad]);
		JslGetLeftY(handledPads[chosenPad]);
	}
	const void Input::GetRightXY(int& x, int& y)
	{
		JslGetRightX(handledPads[chosenPad]);
		JslGetRightY(handledPads[chosenPad]);
	}
	const void Input::GetTriggers(float& l, float& r)
	{
		JslGetLeftTrigger(handledPads[chosenPad]);
		JslGetRightTrigger(handledPads[chosenPad]);
		
	}
	const bool Input::GetButton(ButtonKey key)
	{
		return (bool)(JslGetButtons(handledPads[chosenPad] & key) >> key);
	}
	const void Input::GetMouse(int& x, int& y)
	{
		POINT p;
		GetCursorPos(&p);
		x = p.x;
		y = p.y;
	}
}
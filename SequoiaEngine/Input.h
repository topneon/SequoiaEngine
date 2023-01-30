#pragma once
#include "Object.h"
#include "framework.h"
#include "JoyShockLibrary.h"

namespace seq {
	typedef uint8_t ButtonState;
	typedef uint8_t ButtonKey;
	typedef int HandledGamepad;

	class Input : public Object
	{
		ButtonState buttonState[255]; // 0-254
		ButtonState buttonCheck[255];
		int connectedPads;
		HandledGamepad handledPads[4]; // dynamic memory is unsafe, lets use static array
		int chosenPad;
	public:
		void Init() override;
		void Tick() override;
		void Destroy() override;
		// KEYBOARD
		const bool GetKey(ButtonKey key);
		const bool GetKeyDown(ButtonKey key);
		const bool GetKeyUp(ButtonKey key);
		// GAMEPAD
		const void GetLeftXY(int& x, int& y);
		const void GetRightXY(int& x, int& y);
		const void GetTriggers(float& l, float& r);
		const bool GetButton(ButtonKey key);
		// MOUSE
		const void GetMouse(int& x, int& y);
	};

	typedef Input Keyboard;
	typedef Input Mouse;
	typedef Input Gamepad;
	typedef Gamepad Dualshock;
	typedef Gamepad PSController;
	typedef Gamepad PlayStationController;
	typedef Gamepad XboxController;
}
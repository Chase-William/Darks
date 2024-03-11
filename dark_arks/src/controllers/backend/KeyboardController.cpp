#include "KeyboardController.h"

bool KeyboardController::Keystroke(Key key, KeyState state)
{
	// Key should never be empty
	assert(static_cast<int>(key) != 0);
	// State should never be empty
	assert(static_cast<int>(state) != 0);	

	// Get state as an int for bitwise checking
	int state_num = static_cast<int>(state);
	// Determine if pressed and/or release are request for the keystroke
	bool pressed = state_num & static_cast<int>(KeyState::Pressed);
	bool released = state_num & static_cast<int>(KeyState::Released);

	INPUT input{};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = static_cast<WORD>(key);

	// The result var starts at 0 and should end at 0
	// Otherwise an error occured and false shall be returned
	int result = 0;

	if (pressed) {
		// Key down
		result += SendInput(1, &input, sizeof(input));		
	}
	if (released) {
		// Key up
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		result -= SendInput(1, &input, sizeof(input));
	}
	
	return !result;
}

#include "KeyboardController.h"

namespace Darks::Controllers {
	void KeyboardController::Keystroke(IO::Key key, KeyState state) const
	{
		DARKS_TRACE(fmt::format("Requested to send key {} with state {}.", static_cast<unsigned char>(key), static_cast<int>(state)));

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
		if (pressed) {
			// Key down
			if (!SendInput(1, &input, sizeof(input))) {
				DARKS_ERROR(fmt::format("Was unable to send key press of {}.", static_cast<unsigned char>(key)));
				throw std::runtime_error(fmt::format("Was unable to send key press of {}.", static_cast<unsigned char>(key)));
			}
		}
		if (released) {
			// Key up
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			if (!SendInput(1, &input, sizeof(input))) {
				DARKS_ERROR(fmt::format("Was unable to send key release of {}.", static_cast<unsigned char>(key)));
				throw std::runtime_error(fmt::format("Was unable to send key release of {}.", static_cast<unsigned char>(key)));
			}
		}

		DARKS_TRACE(fmt::format("Keystroke sent."));
	}

	void KeyboardController::Keystrokes(const std::string& text) const
	{
		DARKS_TRACE(fmt::format("Sending keystrokes: {}.", text));

		int len = text.length();

		assert(len != 0);

		// Do not process an empty string
		if (!len) return;

		std::vector<INPUT> inputs{};

		INPUT input{};
		input.type = INPUT_KEYBOARD;
		for (int i = 0; i < len; i++) {
			// Take the character and copy it to the virtual key space
			input.ki.dwFlags = 0;
			input.ki.wVk = text[i];
			inputs.push_back(input);
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			inputs.push_back(input);
		}

		auto size = inputs.size();
		auto size_of = sizeof(INPUT);
		auto size_of_instance = sizeof(input);

		UINT result = SendInput(inputs.size(), inputs.data(), sizeof(input));

		// Success if all inputs successfully merged into input stream
		if (result != inputs.size()) {
			DARKS_ERROR(fmt::format("Was able to send {:d} keystrokes out of {:d}.", result, inputs.size()));
			throw std::runtime_error("Was unable to send all keystrokes.");
		}
	}
}
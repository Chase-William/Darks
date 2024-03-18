#pragma once

#include <Windows.h>
#include <vector>
#include <cassert>
#include <string>
#include <locale>
#include <expected>

#include "../Log.h"
#include "../io/VirtualInput.h"
#include "../Exceptions.h"

/// <summary>
/// The state of a given keystroke.
/// </summary>
enum class KeyState {
	Pressed = 0x0001,
	Released = 0x0002
};

static KeyState operator|(KeyState v1, KeyState v2) {
	// Overload to allow bitwise combinations 
	return static_cast<KeyState>(static_cast<int>(v1) + static_cast<int>(v2));
}

/// <summary>
/// Provides general keyboard utilities.
/// </summary>
class KeyboardController {
public:
	/// <summary>
	/// Performs a keystroke with the specificed state.
	/// </summary>
	/// <param name="key">Keystroke to be simulated.</param>
	/// <param name="state">State of keystroke to be sent.</param>
	/// <returns>True if all keystrokes executed as expected, false otherwise.</returns>
	void Keystroke(Key key, KeyState state = KeyState::Pressed | KeyState::Released);
	/// <summary>
	/// Performs a keystroke for all specified characters.
	/// </summary>
	/// <param name="text">A string of A-Z or 0-9 or space characters.</param>
	/// <returns>True if all inputs merged successfully into input stream, false otherwise.</returns>
	void Keystrokes(const std::string& text);
};


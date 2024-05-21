#include "MouseController.h"

#include <cassert>

#include "../Log.h"

namespace Darks::Controller {
	const unsigned short NORMALIZED_SCREEN_SPACE_MAX_VALUE = 65535;

	MouseController::MouseController() {
		// Get the width and height of the screen
		primary_screen_width_ = GetSystemMetrics(SM_CXSCREEN);
		primary_screen_height_ = GetSystemMetrics(SM_CYSCREEN);

		assert(primary_screen_height_ && primary_screen_width_);

		if (!primary_screen_width_ || !primary_screen_height_) {
			auto msg = "Failed to get screen width and/or height.";
			DARKS_ERROR(msg);
			throw std::runtime_error(msg);
		}
	}

	static int ToNative(ClickType click_type) {
		// Map enum to click type down + up
		switch (click_type) {
		case ClickType::Left:
			return MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
		case ClickType::Right:
			return MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
		case ClickType::Middle:
			return MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
		default:
			auto msg = std::format("The given click type {} is invalid.", static_cast<int>(click_type));
			DARKS_ERROR(msg);
			throw std::invalid_argument(msg);
		}
	}

	void MouseController::Click(ClickType click_type) const {
		INPUT input{};
		input.type = INPUT_MOUSE;

		input.mi.dwFlags = ToNative(click_type);

		if (!SendInput(1, &input, sizeof(input))) {
			auto msg = std::format("Was unable to send a mouse click of type {}.", static_cast<int>(click_type));
			DARKS_ERROR(msg);
			throw std::runtime_error(msg);
		}
	}

	void MouseController::Click(int x, int y, ClickType click_type) const {
		INPUT input{};
		input.type = input.type = INPUT_MOUSE;

		// Calculate normalized value of given x and y in respect to NORMALIZED_SCREEN_SPACE_MAX_VALUE
		// See: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-mouseinput#remarks
		input.mi.dx = MulDiv(x, NORMALIZED_SCREEN_SPACE_MAX_VALUE, this->primary_screen_width_);
		input.mi.dy = MulDiv(y, NORMALIZED_SCREEN_SPACE_MAX_VALUE, this->primary_screen_height_);
		// X and Y are absolute positioned to screen space
		input.mi.dwFlags =
			ToNative(click_type) |
			MOUSEEVENTF_ABSOLUTE | // Coordinates are absolute
			// MOUSEEVENTF_VIRTUALDESK | // Map to entire virtual desktop (all monitors included)
			MOUSEEVENTF_MOVE; // Signify movement has occured and positioning should be used

		if (!SendInput(1, &input, sizeof(input))) {
			auto msg = std::format("Was unable to send a mouse click at x: {:d}, y: {:d} of type {}.", x, y, static_cast<int>(click_type));
			DARKS_ERROR(msg);
			throw std::runtime_error(msg);
		}
	}
}
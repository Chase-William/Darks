#include "MouseController.h"

const unsigned short NORMALIZED_SCREEN_SPACE_MAX_VALUE = 65535;

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
		throw std::invalid_argument(std::format("The given click type {} is invalid.", static_cast<int>(click_type)));
	}	
}

bool MouseController::Click(ClickType click_type) const
{
	INPUT input{};
	input.type = INPUT_MOUSE;

	input.mi.dwFlags = ToNative(click_type);

	return SendInput(1, &input, sizeof(input));
}

bool MouseController::Click(int x, int y, ClickType click_type) const
{
	INPUT input{};
	input.type = INPUT_MOUSE;

	// Calculate normalized value of given x and y in respect to NORMALIZED_SCREEN_SPACE_MAX_VALUE
	// See: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-mouseinput#remarks
	input.mi.dx = NORMALIZED_SCREEN_SPACE_MAX_VALUE / primary_screen_width_ * x;
	input.mi.dy = NORMALIZED_SCREEN_SPACE_MAX_VALUE / primary_screen_height_ * y;

	// X and Y are absolute positioned to screen space
	input.mi.dwFlags = ToNative(click_type) | 
		MOUSEEVENTF_ABSOLUTE | // Coordinates are absolute
		// MOUSEEVENTF_VIRTUALDESK | // Map to entire virtual desktop (all monitors included)
		MOUSEEVENTF_MOVE; // Signify movement has occured and positioning should be used

	return SendInput(1, &input, sizeof(input));
}

MouseController::MouseController(int primary_screen_width, int primary_screen_height) :
	primary_screen_width_(primary_screen_width),
	primary_screen_height_(primary_screen_height)
{ }

MouseController MouseController::New() {
	// Get the width and height of the screen
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	if (!width || !height)
		throw std::runtime_error("Failed to get screen width and/or height.");
	return MouseController(width, height);
}
#pragma once

#include <Windows.h>
#include <stdexcept>
#include <format>

/// <summary>
/// Represents a mouse click type.
/// </summary>
enum class ClickType {
	Left,
	Right,
	Middle
};

/// <summary>
/// Provides general mouse usage utilities.
/// </summary>
class MouseController
{
public:
	static MouseController New();

	/// <summary>
	/// Clicks using the cursor's current position.
	/// </summary>
	/// <param name="click_type">The type of mouse click to be performed.</param>
	/// <returns>True if click was successfully inserted into input stream, false otherwise.</returns>
	bool Click(ClickType click_type = ClickType::Left) const;
	/// <summary>
	/// Clicks at the position given on the primary screen.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="click_type">The type of mouse click to be performed.</param>
	/// <returns>True if click was successfully inserted into input stream, false otherwise.</returns>
	bool Click(int x, int y, ClickType click_type = ClickType::Left) const;

private:
	MouseController(int primary_screen_width, int primary_screen_height);

	int primary_screen_width_;
	int primary_screen_height_;
};


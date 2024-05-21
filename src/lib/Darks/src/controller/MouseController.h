#ifndef DARKS_CONTROLLERS_MOUSECONTROLLER_H_
#define DARKS_CONTROLLERS_MOUSECONTROLLER_H_

#include "../io/Pixel.h"

namespace Darks::Controller {
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
	class MouseController {
	public:
		MouseController();

		/// <summary>
		/// Clicks using the cursor's current position.
		/// </summary>
		/// <param name="click_type">The type of mouse click to be performed.</param>
		/// <returns>True if click was successfully inserted into input stream, false otherwise.</returns>
		void Click(ClickType click_type = ClickType::Left) const;
		/// <summary>
		/// Clicks at the position given on the primary screen.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="click_type">The type of mouse click to be performed.</param>
		/// <returns>True if click was successfully inserted into input stream, false otherwise.</returns>
		void Click(int x, int y, ClickType click_type = ClickType::Left) const;
		void Click(IO::Point pos, ClickType click_type = ClickType::Left) const { return Click(pos.x, pos.y, click_type); }

	private:
		int primary_screen_width_;
		int primary_screen_height_;
	};
}

#endif
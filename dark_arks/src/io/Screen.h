#pragma once

#include <Windows.h>
#include <optional>

#include "../Log.h"
#include "Pixel.h"

namespace Screen {
	/// <summary>
	/// Gets the screen's pixel color at the specified x and y.
	/// </summary>
	/// <returns>The color at the specified coordinates, otherwise an empty optional.</returns>
	static Color GetPixelColor(Point pos) {
		HDC screen_device_ctx = GetDC(NULL);
		// Failed to get dc
		if (!screen_device_ctx) {
			DARKS_ERROR("Was unable to get the screen device context.");
			throw std::runtime_error("Was unable to get the screen device context to inspect pixel color.");
		}
		
		COLORREF color_ref = GetPixel(screen_device_ctx, pos.x, pos.y);
		
		// Failed to get pixel color
		if (color_ref == CLR_INVALID) {
			DARKS_ERROR("The result from win32 sys call GetPixel returned the invalid flag CLR_INVALID.");
			throw std::runtime_error("Color returned from win32 GetPixel sys call was invalid (CLR_INVALID).");
		}
		
		if (!ReleaseDC(NULL, screen_device_ctx)) {
			DARKS_ERROR("Was unable to release the screen device context.");
		}
		
		Color color{};
		color.red = GetRValue(color_ref);
		color.green = GetGValue(color_ref);
		color.blue = GetBValue(color_ref);
		
		return color;
	}
}
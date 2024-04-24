#ifndef DARKS_IO_WINDOW_H_
#define DARKS_IO_WINDOW_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "../Log.h"

namespace Darks::IO::Window {
	inline bool FocusWindow() {
		// ----------------------------------------- Add converter
		auto window_name_wide = L"ArkAscended";
		auto window_name = "ArkAscended";
		
		HWND hwnd = FindWindow(nullptr, window_name_wide);
		// Return false if we fail to find the ark window
		if (!hwnd) {
			DARKS_WARN(std::format("Was unable to find window {}.", window_name));
			return false;
		}
		
		bool status = SetForegroundWindow(hwnd); // Focus the ASA window for user input

		if (status) {
			DARKS_INFO(std::format("Was unable to set window {} as foreground window.", window_name));
		}
		else {
			DARKS_INFO(std::format("Successfully set window {} as foreground window.", window_name));
		}

		return status;
	}
}

#endif
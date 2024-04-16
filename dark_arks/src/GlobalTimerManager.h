#ifndef DARKS_GLOBALTIMERMANAGER_H_
#define DARKS_GLOBALTIMERMANAGER_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <optional>
#include <functional>
#include <map>
#include <stdexcept>

#include "Log.h"

class GlobalTimerManager {
public:
	GlobalTimerManager() {
		// SetUserObjectInformationW(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION,. );
	}

	/// <summary>
	/// Registers a repeating timer with a callback and specified interval.
	/// </summary>
	/// <param name="callback">The callback function to be invoked when elapsed.</param>
	/// <param name="interval"></param>
	/// <returns>The id associated with the registered timer, or an empty optional otherwise.</returns>
	std::optional<int> Register(std::function<void()> callback, int interval);
	/// <summary>
	/// Unregisters a repeating timer.
	/// </summary>
	/// <param name="id">Id of the timer to be unregistered.</param>
	/// <returns>True if successful, false otherwise.</returns>
	bool Unregister(int id);	
};

#endif
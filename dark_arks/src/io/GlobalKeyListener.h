#ifndef DARKS_IO_GLOBALKEYLISTENER_H_
#define DARKS_IO_GLOBALKEYLISTENER_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <map>
#include <functional>
#include <optional>
#include <iterator>
#include <stdexcept>
#include <cassert>

#include "../Log.h"
#include "VirtualInput.h"
#include "../controllers/IKeyListener.h"

/// <summary>
/// Allows listening to low level keyboard strokes.
/// 
/// **Note - No more than once instance of this class should be created in memory.
/// </summary>
class GlobalKeyListener {
public:
	GlobalKeyListener();

	/// <summary>
	/// Subscribes a listener and registers the hook if appropriate.
	/// </summary>
	/// <param name="handler_func"></param>
	void operator+=(IKeyListener* listener);
	/// <summary>
	/// Unsubscribes a listener and unregisters the hook if appropriate.
	/// </summary>
	/// <param name="handler_func"></param>
	void operator-=(IKeyListener* listener);

private:
	/// <summary>
	/// A map of handlers where any given key x can have y number of handlers.
	/// </summary>
	std::vector<IKeyListener*> listeners_{};

	// Used when registering a low level key hook to listen for non-hotkeys that should alter state of the auto-walker
	HHOOK low_level_keyboard_hook_ = NULL;
};

#endif
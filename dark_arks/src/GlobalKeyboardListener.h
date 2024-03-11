#pragma once

#include <Windows.h>
#include <map>
#include <functional>
#include <optional>
#include <iterator>
#include <stdexcept>
#include <cassert>

#include "controllers/VirtualInput.h"
#include "controllers/IKeyboardListenable.h"

/// <summary>
/// Allows listening to low level keyboard strokes.
/// 
/// **Note - No more than once instance of this class should be created in memory.
/// </summary>
class GlobalKeyboardListener {
public:
	GlobalKeyboardListener();

	/// <summary>
	/// Subscribes a listener and registers the hook if appropriate.
	/// </summary>
	/// <param name="handler_func"></param>
	void operator+=(IKeyboardListenable* listener);
	/// <summary>
	/// Unsubscribes a listener and unregisters the hook if appropriate.
	/// </summary>
	/// <param name="handler_func"></param>
	void operator-=(IKeyboardListenable* listener);

private:
	/// <summary>
	/// A map of handlers where any given key x can have y number of handlers.
	/// </summary>
	std::vector<IKeyboardListenable*> listeners_{};

	// Used when registering a low level key hook to listen for non-hotkeys that should alter state of the auto-walker
	HHOOK low_level_keyboard_hook_ = NULL;
};


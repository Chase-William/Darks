#ifndef DARKS_IO_GLOBALKEYLISTENER_H_
#define DARKS_IO_GLOBALKEYLISTENER_H_

#include "../Framework.h"

#include <vector>

#include "../Log.h"
#include "VirtualInput.h"
#include "../controller/IKeyListener.h"

namespace Darks::IO {
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
		void operator+=(Darks::Controller::IKeyListener* listener);
		/// <summary>
		/// Unsubscribes a listener and unregisters the hook if appropriate.
		/// </summary>
		/// <param name="handler_func"></param>
		void operator-=(Darks::Controller::IKeyListener* listener);

	private:
		/// <summary>
		/// A map of handlers where any given key x can have y number of handlers.
		/// </summary>
		std::vector<Darks::Controller::IKeyListener*> listeners_{};

		// Used when registering a low level key hook to listen for non-hotkeys that should alter state of the auto-walker
		HHOOK low_level_keyboard_hook_ = NULL;
	};
}

#endif
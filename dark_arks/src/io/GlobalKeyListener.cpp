#include "GlobalKeyListener.h"

// A pointer to the handler collection to be used for invoking callbacks
// **NOTE, this should be a vec of vecs if we plan to use multipled instances of GlobalKeboardListener throughout the application
static std::vector<IKeyListener*>* handler_collection;

static LRESULT CALLBACK LowLevelKeyboardProc(
	_In_ int    n_code,
	_In_ WPARAM w_param,
	_In_ LPARAM l_param
) {
	// If bigger than or equal to 0, we check value
	if (n_code > -1) {
		KBDLLHOOKSTRUCT* key_info = reinterpret_cast<KBDLLHOOKSTRUCT*>(l_param);

		// Inform listeners of keystroke
		for (auto listener : *handler_collection)
			listener->HandleKeystroke(static_cast<Key>(key_info->vkCode));
	}

	return CallNextHookEx(NULL, n_code, w_param, l_param);
}

GlobalKeyListener::GlobalKeyListener()
{
	handler_collection = &listeners_;
}

void GlobalKeyListener::operator+=(IKeyListener* listener) {
	// Check if registration is required
	if (!low_level_keyboard_hook_) {
		// Needs to register
		low_level_keyboard_hook_ = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);

		// Should have valid hook			
		assert(low_level_keyboard_hook_);

		if (!low_level_keyboard_hook_) {
			DARKS_ERROR("Was unable to register a low level keyboard hook for the auto walker.");
			throw std::runtime_error("Failed to create WH_KEYBOARD_LL hook.");
		}
	}

	listeners_.push_back(listener);
}

void GlobalKeyListener::operator-=(IKeyListener* listener) {
	// Attempts to unregister handlers when the handlers vec is empty shouldnt happen
	assert(!listeners_.empty());

	// Locate the handler in the handler vec
	auto result_iter = std::find(listeners_.begin(), listeners_.end(), listener);

	// Unable to find a handler that is attempting to be removed
	assert(result_iter != listeners_.end());

	listeners_.erase(result_iter);

	// Check if unregistration is required
	if (listeners_.empty()) {
		// The HHOOK should never be NULL if listers were registered
		assert(low_level_keyboard_hook_);

		if (!UnhookWindowsHookEx(low_level_keyboard_hook_)) {
			DARKS_ERROR("Was unable to unregister the auto walker's low level keyboard hook.");
			throw std::runtime_error("Failed to unregister WH_KEYBOARD_LL hook.");
		}

		// Reset hook value
		low_level_keyboard_hook_ = NULL;
	}
}

//
//
//	What if I overload the += and -= operators to sub and unsub a function
//		The function is invoked whenever a key is pressed and receives that key as a argument
//		This removes the need to map to keys and function ptr, but does make the client do more work
//
//

//std::optional<GKL_Key> GlobalKeyListener::Register(std::function<void()> handler, Key key)
//{
//	if (!hooked_) {
//		// If not hooked, hook the listener
//		low_level_keyboard_hook_ = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
//
//		// The hook returned is valid, registration successful
//		assert(low_level_keyboard_hook_);
//	}
//
//	int id = static_cast<int>(key);
//	
//	if (auto r = listeners_.find(id); r != listeners_.end()) {
//		// If handler already contains an entry for the given key, push this handler onto its value vec
//		r->second.push_back(handler);
//	}
//	else {
//		// Insert new entry using vec initializer given the provided handler
//		listeners_.insert({ id, { handler } });
//	}
//		
//
//	return std::optional<GKL_Key>({ id, &handler });
//}
//
//bool GlobalKeyListener::Unregister(int id)
//{
//	return false;
//}

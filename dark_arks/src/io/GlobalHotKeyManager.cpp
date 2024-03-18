#include "GlobalHotKeyManager.h"

void GlobalHotKeyManager::Dispatch(int id) {	
	// Locate a handler if one exists
	if (auto handler = listeners_.find(id); handler != listeners_.end()) {
		// Fire the handler
		handler->second();
		return;
	}

	DARKS_CRITICAL(fmt::format("The given hotkey id of {:d} does not have a associated handler function! A hotkey was pressed that didn't have a callback!", id));
	// Throw a runtime exception if we are failing to find a registered hotkey_ in our callback map
	throw std::runtime_error(std::format("Was unable to find a registered hotkey_ and its handler for the given id of {}.", id));
}

bool GlobalHotKeyManager::Register(	
	HotKey& hotkey,
	std::function<void()> handler
) {
	DARKS_TRACE(std::format("Registering hotkey with key: {} and mods: {}.", static_cast<unsigned char>(hotkey.key_), static_cast<int>(hotkey.mods_)));

	// Generate string representing mods+key and create hash as id
	std::string str{};
	// Get mod as uint for bitwise flag checks
	unsigned int mods_as_uint = static_cast<int>(hotkey.mods_);

	// Checks
	if ((mods_as_uint & static_cast<unsigned int>(Modifiers::Shift)) == static_cast<unsigned int>(Modifiers::Shift)) {
		str += "shift+";
	}
	if ((mods_as_uint & static_cast<unsigned int>(Modifiers::Ctrl)) == static_cast<unsigned int>(Modifiers::Ctrl)) {
		str += "ctrl+";
	}
	if ((mods_as_uint & static_cast<unsigned int>(Modifiers::Alt)) == static_cast<unsigned int>(Modifiers::Alt)) {
		str += "alt+";
	}
	if ((mods_as_uint & static_cast<unsigned int>(Modifiers::Win)) == static_cast<unsigned int>(Modifiers::Win)) {
		str += "win+";
	}

	// Convert enum to character
	str += static_cast<char>(hotkey.key_);

	int id = std::hash<std::string>{}(str);

	// BOOL result = RegisterHotKey(nullptr, id, mods_as_uint, static_cast<UINT>(hotkey.key_)) ? std::optional(id) : std::nullopt;

	// Before leaving, if successful, save the handler as a callback
	if (RegisterHotKey(nullptr, id, mods_as_uint, static_cast<UINT>(hotkey.key_))) {
		listeners_.insert({ id, handler });
		hotkey.id_ = id;
		DARKS_INFO(fmt::format("Registered hotkey: key: {:c} mods: {:d} with id: {:d}.", static_cast<char>(hotkey.key_), static_cast<int>(hotkey.mods_), id));
		// Reg successful
		return true;
	}

	DARKS_ERROR(fmt::format("Was unable to register hotkey: key: {:c} mods: {:d}", static_cast<char>(hotkey.key_), static_cast<int>(hotkey.mods_)));
	// Failed to reg
	return false;
}

bool GlobalHotKeyManager::Unregister(HotKey& hotkey) {
	if (!hotkey.id_) {
		DARKS_INFO(fmt::format("Attempt to unregister already unregistered hotkey {:d} was averted.", hotkey.id_));
		return true; // This is not registered
	}

	// Remove that handler
	listeners_.erase(hotkey.id_);

	if (UnregisterHotKey(nullptr, hotkey.id_)) {
		// Unreg successful
		DARKS_INFO(fmt::format("Hotkey {:d} unregistered.", hotkey.id_));
		hotkey.id_ = 0;		
		return true;
	}

	DARKS_ERROR(fmt::format("Was unable to unregister hotkey {:d}.", hotkey.id_));
	// Failed to unreg
	return false;
}

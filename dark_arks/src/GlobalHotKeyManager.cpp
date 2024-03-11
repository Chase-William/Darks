#include "GlobalHotKeyManager.h"

void GlobalHotKeyManager::Dispatch(int id) {	
	// Locate a handler if one exists
	if (auto handler = listeners_.find(id); handler != listeners_.end()) {
		// Fire the handler
		handler->second();
		return;
	}

	// Throw a runtime exception if we are failing to find a registered hotkey_ in our callback map
	throw std::runtime_error(std::format("Was unable to find a registered hotkey_ and its handler for the given id of {}.", id));
}

std::optional<int> GlobalHotKeyManager::Register(
	std::function<void()> handler,
	Key key, 
	Modifiers mods)
{
	// Generate string representing mods+key and create hash as id
	std::string str{};
	// Get mod as uint for bitwise flag checks
	unsigned int mods_as_uint = static_cast<int>(mods);

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
	str += static_cast<char>(key);

	int id = std::hash<std::string>{}(str);

	auto result = RegisterHotKey(nullptr, id, mods_as_uint, static_cast<UINT>(key)) ? std::optional(id) : std::nullopt;

	// Before leaving, if successful, save the handler as a callback
	if (result.has_value()) {
		listeners_.insert({ id, handler });
	}

	return result;
}

bool GlobalHotKeyManager::Unregister(int id)
{
	// Assert id given is not 0
	assert(!id);

	// Remove that handler
	listeners_.erase(id);
	return UnregisterHotKey(nullptr, id);
}

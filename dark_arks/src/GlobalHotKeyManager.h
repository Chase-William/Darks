#pragma once

#include <Windows.h>
#include <optional>
#include <unordered_map>
#include <string>
#include <functional>
#include <map>
#include <cassert>
#include <stdexcept>
#include <format>

#include "controllers/VirtualInput.h"

/// <summary>
/// Allows registration, unregistration, and listening of hotkeys.
/// </summary>
class GlobalHotKeyManager
{
public:
	/// <summary>
	/// Registers a hotkey_ and returns an id that can later be used to unregister the hotkey_.
	/// </summary>
	/// <param name="key"></param>
	/// <param name="mods"></param>
	/// <returns></returns>
	std::optional<int> Register(
		std::function<void()> handler,
		Key key, 
		Modifiers mods = Modifiers::None); // Should register take a callback, should that be a closure? give proper access to this*
	/// <summary>
	/// Unregisters the hotkey_ associated with the id.
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	bool Unregister(int id);

	void Dispatch(int id);
private:
	std::map<int, std::function<void()>> listeners_{};
};


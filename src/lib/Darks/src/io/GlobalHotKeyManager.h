#ifndef DARKS_IO_GLOBALHOTKEYMANAGER_H_
#define DARKS_IO_GLOBALHOTKEYMANAGER_H_

#include <optional>
#include <unordered_map>
#include <string>
#include <functional>
#include <map>
#include <cassert>
#include <stdexcept>
#include <format>

#include "../Log.h"
#include "HotKey.h"
#include "VirtualInput.h"

namespace Darks::IO {
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
		bool Register(
			HotKey& hotkey,
			std::function<void()> handler);
		/// <summary>
		/// Unregisters the hotkey_ associated with the id.
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		bool Unregister(HotKey& hotkey);

		void Dispatch(int id);
	private:
		std::map<int, std::function<void()>> listeners_{};
	};
}

#endif
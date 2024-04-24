#ifndef DARKS_CONTROLLERS_IHOTKEYABLE_H_
#define DARKS_CONTROLLERS_IHOTKEYABLE_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <unordered_map>
#include <optional>

#include "IDisposable.h"
#include "../io/VirtualInput.h"

namespace Darks::Controllers {
	/// <summary>
	/// Implement to use hotkeys.
	/// </summary>
	class IHotKeyable : public IDisposable {
	public:
		virtual bool Register() = 0;
		virtual bool Unregister() = 0;
	};
}

#endif
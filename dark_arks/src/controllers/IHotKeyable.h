#pragma once

#include <Windows.h>
#include <string>
#include <unordered_map>
#include <optional>

#include "IDisposable.h"
#include "../io/VirtualInput.h"

/// <summary>
/// Implement to use hotkeys.
/// </summary>
class IHotKeyable : public IDisposable {
public:
	virtual bool Register() = 0;
	virtual bool Unregister() = 0;
};
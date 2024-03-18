#pragma once

#include <functional>

#include "IDisplayCtrlPanel.h"
#include "IHotKeyable.h"
#include "../io/GlobalHotKeyManager.h"
#include "../io/HotKey.h"

class DisplayControllerConfig {
public:
	HotKey hotkey_ = { Key::Num0 };
};

/// <summary>
/// Controls whether the imgui window control panel is displayed.
/// </summary>
class DisplayController : public IHotKeyable {
public:
	DisplayController(
		DisplayControllerConfig conf, 
		GlobalHotKeyManager& hotkey_manager,
		std::function<void()> handler
	);

	bool Register() override;
	bool Unregister() override;
	void Dispose() override;	
private:
	DisplayControllerConfig conf_;
	GlobalHotKeyManager& hotkey_manager_;
	std::function<void()> handler_;
};


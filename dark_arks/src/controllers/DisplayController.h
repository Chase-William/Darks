#pragma once

#include "IDisplayCtrlPanel.h"
#include "IHotKeyable.h"
#include "../GlobalHotKeyManager.h"

class DisplayControllerConfig {
public:
	std::string hotkey_ = "0";
};

/// <summary>
/// Controls whether the imgui window control panel is displayed.
/// </summary>
class DisplayController : public IDisplayCtrlPanel, public IHotKeyable
{
public:
	DisplayController(DisplayControllerConfig& conf, GlobalHotKeyManager& global_hotkey_manager);

	bool Register();
	bool Unregister();

	bool Show();

private:
	bool show_ = true;
	int hotkey_id_ = 0;
	DisplayControllerConfig& conf_;
	GlobalHotKeyManager& global_hotkey_manager_;
};


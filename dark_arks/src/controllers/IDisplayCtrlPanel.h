#pragma once

#include "imgui/imgui.h"

/// <summary>
/// Implenent to display a control panel for a controller.
/// </summary>
class IDisplayCtrlPanel {
public:
	virtual void DisplayCtrlPanel() {
		ImGui::Text("Default IDisplayCtrlPanel Text.");
	};
};
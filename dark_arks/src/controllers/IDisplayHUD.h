#pragma once

#include "imgui/imgui.h"

/// <summary>
/// Implement to display state updates to imgui background.
/// </summary>
class IDisplayHUD {
public:
	virtual void DisplayHUD() {
		ImGui::GetBackgroundDrawList()->AddText({ 100, 100 }, ImColor(1, 0, 0), "Default Overlay Text!\0");
	}
};
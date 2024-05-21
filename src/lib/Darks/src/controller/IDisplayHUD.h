#ifndef DARKS_CONTROLLERS_IDISPLAYHUD_H_
#define DARKS_CONTROLLERS_IDISPLAYHUD_H_

#include "../Framework.h"

#include "imgui.h"

namespace Darks::Controller {
	/// <summary>
	/// Implement to display state updates to imgui background.
	/// </summary>
	class IDisplayHUD {
	public:
		virtual void DisplayHUD() {
			ImGui::GetBackgroundDrawList()->AddText({ 100, 100 }, ImColor(1, 0, 0), "Default Overlay Text!\0");
		}
	};
}

#endif
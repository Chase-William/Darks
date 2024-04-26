#ifndef DARKS_CONTROLLERS_IDISPLAYCTRLPANEL_H_
#define DARKS_CONTROLLERS_IDISPLAYCTRLPANEL_H_

#include "../Framework.h"

#include "imgui.h"

namespace Darks::Controller {
	/// <summary>
	/// Implenent to display a control panel for a controller.
	/// </summary>
	class IDisplayCtrlPanel {
	public:
		virtual void DisplayCtrlPanel() {
			ImGui::Text("Default IDisplayCtrlPanel Text.");
		};
	};
}

#endif
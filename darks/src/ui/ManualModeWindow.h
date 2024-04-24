#ifndef DARKS_UI_MANUALMODESWINDOW_H_
#define DARKS_UI_MANUALMODESWINDOW_H_

#include <vector>

#include "DarksImguiWindow.h"
#include "../controllers/IDisplayCtrlPanel.h"
#include "../controllers/ProcessController.h"

namespace Darks::UI {
	/// <summary>
	/// An imgui window for interacting with manual mode based options.
	/// </summary>
	class ManualModeWindow : public DarksImguiWindow {
	public:
		ManualModeWindow(
			std::vector<Darks::Controllers::IDisplayCtrlPanel*>& controllers
		);

		void Update() override;

	private:
		std::vector<Darks::Controllers::IDisplayCtrlPanel*>& controllers_;
	};
}

#endif
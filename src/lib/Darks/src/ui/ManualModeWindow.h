#ifndef DARKS_UI_MANUALMODESWINDOW_H_
#define DARKS_UI_MANUALMODESWINDOW_H_

#include <vector>

#include "DarksImguiWindow.h"
#include "../controller/IDisplayCtrlPanel.h"

namespace Darks::UI {
	/// <summary>
	/// An imgui window for interacting with manual mode based options.
	/// </summary>
	class ManualModeWindow : public DarksImguiWindow {
	public:
		ManualModeWindow(
			std::vector<Darks::Controller::IDisplayCtrlPanel*>& controller
		);

		void Update() override;

	private:
		std::vector<Darks::Controller::IDisplayCtrlPanel*>& controllers_;
	};
}

#endif
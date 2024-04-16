#ifndef DARKS_UI_MANUALMODESWINDOW_H_
#define DARKS_UI_MANUALMODESWINDOW_H_

#include <vector>

#include "DarksImguiWindow.h"
#include "../controllers/IDisplayCtrlPanel.h"
#include "../controllers/ProcessController.h"

/// <summary>
/// An imgui window for interacting with manual mode based options.
/// </summary>
class ManualModeWindow : public DarksImguiWindow {
public:
	ManualModeWindow(
		std::vector<IDisplayCtrlPanel*>& controllers,
		ProcessController& process_controller
	);

	void Update() override;

private:
	std::vector<IDisplayCtrlPanel*>& controllers_;
	ProcessController& process_controller_;
};

#endif
#ifndef DARKS_UI_AUTONOMOUSMODEWINDOW_H_
#define DARKS_UI_AUTONOMOUSMODEWINDOW_H_

#include "DarksImguiWindow.h"
#include "../AutonomousWorker.h"

namespace Darks::UI {
	/// <summary>
	/// An imgui window for interacting with autonomous mode based options.
	/// </summary>
	class AutonomousModeWindow : public DarksImguiWindow {
	public:
		AutonomousModeWindow(
			AutonomousWorker& auto_worker
		);

		void Update() override;

	private:
		AutonomousWorker& auto_worker_;
	};
}

#endif
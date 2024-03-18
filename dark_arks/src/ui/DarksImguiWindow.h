#pragma once

#include <Windows.h>
#include <vector>

#include "../controllers/IDisplayCtrlPanel.h"
#include "../controllers/AutoClickerController.h"
#include "../controllers/AutoWalkerController.h"
#include "../controllers/MouseController.h"
#include "../controllers/KeyboardController.h"
#include "../controllers/InventoryController.h"
#include "../AutonomousWorker.h"

class DarksImguiWindow {
public:
	DarksImguiWindow(
		std::vector<IDisplayCtrlPanel*>& controllers,
		AutonomousWorker& auto_worker);
	/*DarksImguiWindow(DarksImguiWindow& window) {
		auto_worker_ = window.auto_worker_;
	}*/

	static void Init(HWND hwnd);

	void Update();

	class {
		bool value;
	public:
		bool& operator = (const bool& v) {
			// Check if redundent set
			if (value == v) return value;
			// Becoming visible and enable interactivity
			if (v) EnableInteractivity();			
			// Becoming hidden and disable interactivity allowing clicks to passthrough
			else DisableInteractivity();			
			return value = v;
		}
		operator bool() const { return value; }
	} IsVisible; // Updates the native window when state changes

private:
	AutonomousWorker& auto_worker_;
	std::vector<IDisplayCtrlPanel*>& controllers_;

	static void EnableInteractivity();
	static void DisableInteractivity();
};


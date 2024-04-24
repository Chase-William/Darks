#ifndef DARKS_UI_DARKSIMGUIWINDOW_H_
#define DARKS_UI_DARKSIMGUIWINDOW_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <vector>
#include <initializer_list>

#include "../controllers/AutoClickerController.h"
#include "../controllers/AutoWalkerController.h"
#include "../controllers/MouseController.h"
#include "../controllers/KeyboardController.h"
#include "../controllers/InventoryController.h"
#include "../AutonomousWorker.h"

namespace Darks::UI {
	/// <summary>
	/// The base window used for all imgui window implementation.
	/// </summary>
	class DarksImguiWindow {
	public:
		DarksImguiWindow(std::string window_name
		) :
			window_name_(window_name)
		{ }

		static void Init(HWND hwnd);

		virtual void Update() = 0;

		class {
			bool value = true;
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

		/// <summary>
		/// Prevents all windows from displaying visible or not.
		/// </summary>
		static void Disable();
		/// <summary>
		/// Allows all windows to display if visible.
		/// </summary>
		static void Enable();

		static void Update(std::initializer_list<DarksImguiWindow*> windows);

	protected:
		bool IsMouseOver();

	private:
		std::string window_name_;

		static void EnableInteractivity();
		static void DisableInteractivity();
	};
}

#endif
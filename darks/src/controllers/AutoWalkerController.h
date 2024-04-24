#ifndef DARKS_CONTROLLERS_AUTOWALKERCONTROLLER_H_
#define DARKS_CONTROLLERS_AUTOWALKERCONTROLLER_H_

#include <optional>
#include <expected>

#include "imgui/imgui.h"

#include "../Log.h"
#include "IDisplayCtrlPanel.h"
#include "IHotKeyable.h"
#include "KeyboardController.h"
#include "../io/GlobalHotKeyManager.h"
#include "../io/GlobalKeyListener.h"
#include "../io/HotKey.h"
#include "IKeyListener.h"
#include "IDisplayHUD.h"

namespace Darks::Controllers {
	class AutoWalkerConfig {
	public:
		std::optional<IO::HotKey> hotkey_ = std::nullopt;
		IO::Key walk_forward_key_ = IO::Key::W;
		IO::Key walk_backward_key_ = IO::Key::S;
		IO::Modifiers sprint_key_ = IO::Modifiers::Shift;		
	};

	/// <summary>
	/// Provides an automatic walking/sprinting tool.
	/// </summary>
	class AutoWalkerController : public IDisplayCtrlPanel, public IDisplayHUD, public IHotKeyable, public IKeyListener {
	public:
		AutoWalkerController(
			AutoWalkerConfig conf,
			IO::GlobalHotKeyManager& hotkey_manager,
			IO::GlobalKeyListener& keyboard_listener);

		void DisplayCtrlPanel() override;
		void DisplayHUD() override;
		bool Register() override;
		bool Unregister() override;
		void HandleKeystroke(IO::Key key) override;
		void Dispose() override;

	private:
		bool running_ = false;

		AutoWalkerConfig conf_;
		KeyboardController keyboard_controller_{};
		IO::GlobalHotKeyManager& hotkey_manager_;
		IO::GlobalKeyListener& keyboard_listener_;

		void Walk();
	};
}

#endif
#ifndef DARKS_CONTROLLERS_DISPLAYCONTROLLER_H_
#define DARKS_CONTROLLERS_DISPLAYCONTROLLER_H_

#include <functional>

#include "IDisplayCtrlPanel.h"
#include "IHotKeyable.h"
#include "../io/GlobalHotKeyManager.h"
#include "../io/HotKey.h"

namespace Darks::Controller {
	class DisplayControllerConfig {
	public:
		IO::HotKey hotkey_ = { IO::Key::Num0 };
	};

	/// <summary>
	/// Controls whether the imgui window control panel is displayed.
	/// </summary>
	class DisplayController : public IHotKeyable {
	public:
		DisplayController(
			DisplayControllerConfig conf,
			IO::GlobalHotKeyManager& hotkey_manager,
			std::function<void()> handler
		);

		bool Register() override;
		bool Unregister() override;
		void Dispose() override;
	private:
		DisplayControllerConfig conf_;
		IO::GlobalHotKeyManager& hotkey_manager_;
		std::function<void()> handler_;
	};
}

#endif
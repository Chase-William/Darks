#ifndef DARKS_CONTROLLERS_GENERALCONTROLLER_H_
#define DARKS_CONTROLLERS_GENERALCONTROLLER_H_

#include "../io/VirtualInput.h"
#include "KeyboardController.h"

namespace Darks::Controller {
	class GeneralConfig {
	public:
		IO::Key use_key_ = IO::Key::E;
	};

	class GeneralController {
	public:
		GeneralController(GeneralConfig conf) : conf_(conf) { }

		void Use() const {
			keyboard_controller_.Keystroke(conf_.use_key_);
		}

	private:
		GeneralConfig conf_;
		KeyboardController keyboard_controller_{};
	};
}

#endif
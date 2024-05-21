#ifndef DARKS_CONTROLLERS_CAMERACONTROLLER_H_
#define DARKS_CONTROLLERS_CAMERACONTROLLER_H_

#include "CameraDirection.h"
#include "KeyboardController.h"
#include "../SyncInfo.h"

namespace Darks::Controller {
	class CameraConfig {
	public:
		IO::Key look_left_ = IO::Key::LeftArrow;
		IO::Key look_up_ = IO::Key::UpArrow;
		IO::Key look_right_ = IO::Key::RightArrow;
		IO::Key look_down_ = IO::Key::DownArrow;
	};

	class CameraController {
	public:
		CameraController(CameraConfig conf_);

		void Pivot(SyncInfo& info, CameraDirection direction, int duration_in_milliseconds) const;

	private:
		CameraConfig conf_;
		KeyboardController keyboard_controller_{};

		void SendPivotKeystrokes(SyncInfo& info, IO::Key key, int duration_in_milliseconds) const;
	};
}

#endif
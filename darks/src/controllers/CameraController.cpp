#include "CameraController.h"

namespace Darks::Controllers {
	CameraController::CameraController(CameraConfig conf) :
		conf_(conf)
	{ }

	void CameraController::SendPivotKeystrokes(SyncInfo& info, IO::Key key, int duration_in_milliseconds) const {
		try {
			keyboard_controller_.Keystroke(key, KeyState::Pressed);
			info.Wait(duration_in_milliseconds);
			keyboard_controller_.Keystroke(key, KeyState::Released);
		}
		catch (std::exception ex) {
			// Ensure key release when exception is caught
			keyboard_controller_.Keystroke(key, KeyState::Released);
			throw ex;
		}
	}

	void CameraController::Pivot(SyncInfo& info, CameraDirection direction, int duration_in_milliseconds) const {
		switch (direction) {
		case CameraDirection::Left:
			SendPivotKeystrokes(info, conf_.look_left_, duration_in_milliseconds);
			break;
		case CameraDirection::Up:
			SendPivotKeystrokes(info, conf_.look_up_, duration_in_milliseconds);
			break;
		case CameraDirection::Right:
			SendPivotKeystrokes(info, conf_.look_right_, duration_in_milliseconds);
			break;
		case CameraDirection::Down:
			SendPivotKeystrokes(info, conf_.look_down_, duration_in_milliseconds);
			break;
		default:
			throw std::invalid_argument(std::format("The provided direction of {} is invalid.", static_cast<int>(direction)));
		}
	}
}
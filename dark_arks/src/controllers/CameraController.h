#ifndef DARKS_CONTROLLERS_CAMERACONTROLLER_H_
#define DARKS_CONTROLLERS_CAMERACONTROLLER_H_

#include "KeyboardController.h"
#include "../SyncInfo.h"

enum class CameraDirection {
	Left,
	Up,
	Right,
	Down,
};

class CameraConfig {
public:
	Key look_left_ = Key::LeftArrow;
	Key look_up_ = Key::UpArrow;
	Key look_right_ = Key::RightArrow;
	Key look_down_ = Key::DownArrow;
};

class CameraController {
public:
	CameraController(CameraConfig conf_);

	void Pivot(SyncInfo& info, CameraDirection direction, int duration_in_milliseconds) const;

private:
	CameraConfig conf_;
	KeyboardController keyboard_controller_{};

	void SendPivotKeystrokes(SyncInfo& info, Key key, int duration_in_milliseconds) const;
};

#endif
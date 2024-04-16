#ifndef DARKS_CONTROLLERS_MOVEMENTCONTROLLER_H_
#define DARKS_CONTROLLERS_MOVEMENTCONTROLLER_H_

#include <stdexcept>

#include "../Log.h"
#include "KeyboardController.h"
#include "../io/VirtualInput.h"
#include "../SyncInfo.h"

enum class Movement {
	Forward,
	Backwards,
	Left,
	Right
};

class MovementConfig {
public:
	Key move_forward_ = Key::W;
	Key move_backwards_ = Key::S;
	Key move_left_ = Key::A;
	Key move_right_ = Key::D;

	Key swim_down_ = Key::C;

	Key crouch_ = Key::C;
};

class MovementController {
public:
	MovementController(MovementConfig conf) : conf_(conf) { }

	/// <summary>
	/// Move the character in a specific direction for a given duration in milliseconds.
	/// </summary>
	/// <param name="info"></param>
	/// <param name="direction"></param>
	/// <param name="for_duration"></param>
	void Move(SyncInfo& info, Movement direction, int for_duration) const;

	void Crouch() const { keyboard_controller_.Keystroke(conf_.crouch_); }

	/// <summary>
	/// Used when swimming to descend in water.
	/// </summary>
	/// <param name="info"></param>
	/// <param name="for_duration"></param>
	void Descend(SyncInfo& info, int for_duration) const;

private:
	MovementConfig conf_;
	KeyboardController keyboard_controller_{};
};

#endif
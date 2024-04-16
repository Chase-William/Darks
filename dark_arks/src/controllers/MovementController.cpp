#include "MovementController.h"

void MovementController::Move(SyncInfo& info, Movement direction, int for_duration) const {
	switch (direction) {
	case Movement::Forward:
		keyboard_controller_.Keystroke(conf_.move_forward_, KeyState::Pressed);
		info.Wait(for_duration);
		keyboard_controller_.Keystroke(conf_.move_forward_, KeyState::Released);
		break;
	case Movement::Left:
		keyboard_controller_.Keystroke(conf_.move_left_, KeyState::Pressed);
		info.Wait(for_duration);
		keyboard_controller_.Keystroke(conf_.move_left_, KeyState::Released);
		break;
	case Movement::Right:
		keyboard_controller_.Keystroke(conf_.move_right_, KeyState::Pressed);
		info.Wait(for_duration);
		keyboard_controller_.Keystroke(conf_.move_right_, KeyState::Released);
		break;
	case Movement::Backwards:
		keyboard_controller_.Keystroke(conf_.move_backwards_, KeyState::Pressed);
		info.Wait(for_duration);
		keyboard_controller_.Keystroke(conf_.move_backwards_, KeyState::Released);
		break;
	default:
		auto msg = std::format("The provided value of {} is invalid for parameter direction.", static_cast<int>(direction));
		DARKS_ERROR(msg);
		throw std::invalid_argument(msg);
	}
}

void MovementController::Descend(SyncInfo& info, int for_duration) const {
	keyboard_controller_.Keystroke(conf_.swim_down_, KeyState::Pressed);
	std::unique_lock<std::mutex> lock(info.mut);
	info.cond_var.wait_for(lock, std::chrono::milliseconds(for_duration));
	keyboard_controller_.Keystroke(conf_.swim_down_, KeyState::Released);
	if (info.shutdown) {
		DARKS_INFO("Initialing manual shutdown.");
		throw ManualShutdownException();
	}
}

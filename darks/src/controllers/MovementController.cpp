#include "MovementController.h"

namespace Darks::Controllers {
	void MovementController::Move(SyncInfo& info, Movement direction, int for_duration) const {		
		assert(for_duration >= 50);

		switch (direction) {
		case Movement::Forward:
			MoveImpl(info, conf_.move_forward_, for_duration);
			break;
		case Movement::Left:
			MoveImpl(info, conf_.move_left_, for_duration);
			break;
		case Movement::Right:
			MoveImpl(info, conf_.move_right_, for_duration);
			break;
		case Movement::Backwards:
			MoveImpl(info, conf_.move_backwards_, for_duration);
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

	void MovementController::MoveImpl(SyncInfo& info, IO::Key move_key, int for_duration) const {
		assert(for_duration > 50);

		/*
		
			This seemingly weird impl is to account for ASA's tendency to ignore keystrokes,
				especially where its a press. This impl provides measures to ensure the keystroke
				press occurs.
		
		*/

		const int MAX_ITERATION_COUNT = 5;
		const int DIVISOR = 50;

		int allowed_iterations = for_duration / DIVISOR;
		int remainder = for_duration % DIVISOR;
		// If there is no remainder after division, send a maximum of 5 key press downs				
		if (!remainder) {
			// Iterate a max of 5 times
			for (int i = 0; i < allowed_iterations && i < MAX_ITERATION_COUNT; i++) {
				// Send keystroke down
				keyboard_controller_.Keystroke(move_key, KeyState::Pressed);
				info.Wait(DIVISOR); // Wait for a moment before continueing
				for_duration -= DIVISOR; // Adjust duration remaining needing to be waited
			}
		}
		// Only send a keystroke down if there is a duration remaining to be waited on
		if (for_duration > 0) {
			keyboard_controller_.Keystroke(move_key, KeyState::Pressed);
			info.Wait(for_duration);			
		}
		keyboard_controller_.Keystroke(move_key, KeyState::Released);
	}
}
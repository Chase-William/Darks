#ifndef DARKS_CONTROLLERS_MOVEMENTCONTROLLER_H_
#define DARKS_CONTROLLERS_MOVEMENTCONTROLLER_H_

#include "KeyboardController.h"
#include "../io/VirtualInput.h"
#include "../SyncInfo.h"

namespace Darks::Controller {
	enum class Movement {
		Forward,
		Backwards,
		Left,
		Right
	};

	class MovementConfig {
	public:
		IO::Key move_forward_ = IO::Key::W;
		IO::Key move_backwards_ = IO::Key::S;
		IO::Key move_left_ = IO::Key::A;
		IO::Key move_right_ = IO::Key::D;

		IO::Key swim_down_ = IO::Key::C;

		IO::Key crouch_ = IO::Key::C;
	};

	class MovementController {
	public:
		MovementController(MovementConfig conf) : conf_(conf) { }

		/// <summary>
		/// Move the character in a specific direction for a given duration in milliseconds.
		/// </summary>
		/// <param name="info"></param>
		/// <param name="direction"></param>
		/// <param name="for_duration">A value in milliseconds greater than or equal to 100.</param>
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

		void MoveImpl(SyncInfo& info, IO::Key move_key, int for_duration) const;
	};
}

#endif
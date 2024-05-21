#ifndef DARKS_CONTROLLERS_MOVEMENTCONTROLLER_H_
#define DARKS_CONTROLLERS_MOVEMENTCONTROLLER_H_

#include "KeyboardController.h"
#include "../io/VirtualInput.h"
#include "../SyncInfo.h"
#include "ILoadable.h"

namespace Darks::Controller {
	enum class Movement {
		Forward,
		Backwards,
		Left,
		Right
	};

	class MovementConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		IO::Key move_forward_ = IO::Key::W;
		IO::Key move_backwards_ = IO::Key::S;
		IO::Key move_left_ = IO::Key::A;
		IO::Key move_right_ = IO::Key::D;

		IO::Key swim_down_ = IO::Key::C;

		IO::Key crouch_ = IO::Key::C;

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}
	};

	static void to_json(nlohmann::json& json, const MovementConfig& conf) {
		json = nlohmann::json({
			{ "move_forward", conf.move_forward_ },
			{ "move_backwards", conf.move_backwards_ },
			{ "move_left", conf.move_left_ },
			{ "move_right", conf.move_right_ },
			{ "swim_down", conf.swim_down_ },
			{ "crouch", conf.crouch_ },
		});
	}

	static void from_json(const nlohmann::json& json, MovementConfig& conf) {
		json.at("move_forward").get_to(conf.move_forward_);
		json.at("move_backwards").get_to(conf.move_backwards_);
		json.at("move_left").get_to(conf.move_left_);
		json.at("move_right").get_to(conf.move_right_);
		json.at("swim_down").get_to(conf.swim_down_);
		json.at("crouch").get_to(conf.crouch_);
	}

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
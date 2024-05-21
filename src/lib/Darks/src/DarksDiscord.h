#ifndef DARKS_DARKSDISCORD_H_
#define DARKS_DARKSDISCORD_H_

#include "Framework.h"

#include <string>
#include <thread>

namespace Darks {
	class DarksDiscord {
	public:
		DarksDiscord(std::string token = "");

		void Start();

		inline dpp::cluster* GetClient() { return &*client_; }

	private:
		std::string token_;
		std::unique_ptr<dpp::cluster> client_;
	};
}

#endif
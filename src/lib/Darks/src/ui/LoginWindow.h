#ifndef DARKS_UI_LOGINWINDOW_H_
#define DARKS_UI_LOGINWINDOW_H_

#include <functional>

#include "DarksImguiWindow.h"
#include "../ServiceContext.h"

namespace Darks::UI {

	class LoginWindow : public DarksImguiWindow {
	public:
		LoginWindow(
			ServiceContext& service_ctx
		);

		void Update() override;	

	private:
		ServiceContext& service_ctx_;
		char user_token_str_[128] = "";
		// char discord_bot_token_str_[128] = "";
		std::unique_ptr<RequestErrorMessage> error_;
	};
}

#endif
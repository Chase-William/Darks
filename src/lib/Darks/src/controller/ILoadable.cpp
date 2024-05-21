#include "ILoadable.h"

namespace Darks::Controller {
	static ServiceContext* service_state_;

	ServiceContext& ILoadable::GetServiceState() {
		return *service_state_;
	}

	void ILoadable::SetServiceState(ServiceContext* service_state) {
		service_state_ = service_state;
	}

	std::tuple<cpr::Url, cpr::Bearer> Darks::Controller::ILoadable::GetLoadRequest(std::string loadable_subdirectory)
	{
		return std::tuple{ 
			cpr::Url { service_state_->GetBaseUrl() + "/" + loadable_subdirectory },
			cpr::Bearer{GetServiceState().GetBearerToken()}
		};
	}
}
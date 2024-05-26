#ifndef DARKS_CONTROLLER_ILOADABLE_H_
#define DARKS_CONTROLLER_ILOADABLE_H_

#include <tuple>
#include <string>

#include "cpr/cpr.h"

#include "../ServiceContext.h"

namespace Darks::Controller {
	class ILoadable {
	public:		
		static ServiceContext& GetServiceState();
		static void SetServiceState(ServiceContext* ctx);

		static std::tuple<cpr::Url, cpr::Bearer, cpr::SslOptions> GetLoadRequest(std::string loadable_subdirectory);
	
		virtual std::string GetUrl() const = 0;
	};
}

#endif
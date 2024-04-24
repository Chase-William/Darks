#pragma once

#include <string>

namespace Darks::Controllers {
	/// <summary>
	/// Implement to specify a required server.
	/// </summary>
	class ISpecifyServer {
	public:
		/// <summary>
		/// Get the server identifier. Most servers can be differentiated numerically, however, in the past there were exceptions i.e. (server ****A and ****B).
		/// </summary>
		/// <returns></returns>
		virtual std::string GetServer() const = 0;
	};

}
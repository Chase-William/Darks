#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Darks {
	class Log {
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger> GetLogger() { return logger_; }

	private:
		static std::shared_ptr<spdlog::logger> logger_;
	};
}

// logging macros
#define DARKS_TRACE(...)    ::Darks::Log::GetLogger()->trace(__VA_ARGS__)
#define DARKS_INFO(...)     ::Darks::Log::GetLogger()->info(__VA_ARGS__)
#define DARKS_WARN(...)     ::Darks::Log::GetLogger()->warn(__VA_ARGS__)
#define DARKS_ERROR(...)    ::Darks::Log::GetLogger()->error(__VA_ARGS__)
#define DARKS_CRITICAL(...) ::Darks::Log::GetLogger()->critical(__VA_ARGS__)
#include "Log.h"

namespace Darks {
	std::shared_ptr<spdlog::logger> Log::logger_;

	void Log::Init() {
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("darks.log", true);
		file_sink->set_pattern("[%T] [%l] %n: %v"); // Pattern from the Cherno's Hazel engine

		// Create and set logger
		logger_ = std::make_shared<spdlog::logger>("darks", file_sink);

		logger_->set_level(spdlog::level::trace);
		logger_->flush_on(spdlog::level::trace);
	}
}
#include "DefaultCrateStation.h"

#include "../../../Log.h"

namespace Darks::Controller::Crate::Station {
	void DefaultCrateStation::Execute(SyncInfo& info) const {
		DARKS_INFO(std::format("Now harvesting crate {}.", conf_.bed_name_));

		// Wait for load phase to end using custom set delay
		info.Wait(conf_.load_delay_);

		// If a navigator is set for navigating to the crate, invoke its navigate function before harvesting
		if (to_crate_navigator_type_)
			to_crate_navigator_type_->Navigate();

		Harvest(info);

		Deposit(info);
		
		DARKS_INFO(std::format("Crate {} harvested successfully.", conf_.bed_name_));
		ExitCrateStation(info);
	}
}
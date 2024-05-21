#include "Crate.h"

namespace Darks::Controller::Crate {
	CrateMessage::CrateMessage(
		CrateMessageType code,
		std::string bed_name,
		std::string description
	) :
		code(code),
		bed_name(bed_name),
		description(description)
	{ }
}
#ifndef DARKS_CONTROLLER_CRATE_NAVIGATOR_INAVIGATEABLE_H_
#define DARKS_CONTROLLER_CRATE_NAVIGATOR_INAVIGATEABLE_H_

namespace Darks::Controller::Crate::Navigator {
	class INavigateable {
	public:
		virtual void Navigate() const = 0;		
	};
}

#endif
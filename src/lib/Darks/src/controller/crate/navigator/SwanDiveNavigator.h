#ifndef DARKS_CONTROLLER_CRATE_NAVIGATOR_SWANDIVENAVIGATOR_H_
#define DARKS_CONTROLLER_CRATE_NAVIGATOR_SWANDIVENAVIGATOR_H_

#include "INavigateable.h"

namespace Darks::Controller::Crate::Navigator {
	class SwanDiveNavigator : public INavigateable {
	public:

		void Navigate() const override;
	};
}

#endif
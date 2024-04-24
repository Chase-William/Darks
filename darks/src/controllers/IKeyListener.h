#ifndef DARKS_CONTROLLERS_IKEYLISTENER_H_
#define DARKS_CONTROLLERS_IKEYLISTENER_H_

#include "../io/VirtualInput.h"

namespace Darks::Controllers {
	class IKeyListener {
	public:
		virtual void HandleKeystroke(IO::Key key) = 0;
	};
}

#endif
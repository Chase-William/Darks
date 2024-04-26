#ifndef DARKS_CONTROLLERS_IHOTKEYABLE_H_
#define DARKS_CONTROLLERS_IHOTKEYABLE_H_

#include "IDisposable.h"

namespace Darks::Controller {
	/// <summary>
	/// Implement to use hotkeys.
	/// </summary>
	class IHotKeyable : public IDisposable {
	public:
		virtual bool Register() = 0;
		virtual bool Unregister() = 0;
	};
}

#endif
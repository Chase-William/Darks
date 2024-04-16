#ifndef DARKS_CONTROLLERS_IKEYLISTENER_H_
#define DARKS_CONTROLLERS_IKEYLISTENER_H_

#include "../io/VirtualInput.h"

class IKeyListener {
public:
	virtual void HandleKeystroke(Key key) = 0;
};

#endif
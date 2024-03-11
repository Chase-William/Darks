#pragma once

#include "VirtualInput.h"

class IKeyboardListenable {
public:
	virtual void HandleKeystroke(Key key) = 0;
};
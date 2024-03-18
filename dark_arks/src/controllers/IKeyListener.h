#pragma once

#include "../io/VirtualInput.h"

class IKeyListener {
public:
	virtual void HandleKeystroke(Key key) = 0;
};
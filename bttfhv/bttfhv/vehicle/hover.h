#pragma once
#include "plugin.h"

enum {
	HOVER_NONE  = 0,
	HOVER_UP,
	HOVER_BOOST
};

int HoverControl(CVehicle* vehicle, bool boost, bool landing, bool damaged);
#pragma once
#include "plugin.h"

bool isPlayerInSphere(CVector p, CVector d, bool onFoot=true);
void UpdateHandling(CVehicle* vehicle);
void UpdateFlyingHandling(CVehicle* vehicle);
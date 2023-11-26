#pragma once
#include <map>
#include "plugin.h"

using namespace std;

extern map<int, int> handlingOverride;

void UpdateHandling();
void UpdateHandling(CVehicle* vehicle);
void UpdateFlyingHandling(CVehicle* vehicle);
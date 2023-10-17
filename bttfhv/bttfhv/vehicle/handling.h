#pragma once
#include <map>
#include "plugin.h"

using namespace std;

extern map<int, int> handlingOverride;

void UpdateHandling(CVehicle* vehicle);
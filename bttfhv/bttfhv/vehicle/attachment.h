#pragma once
#include <map>

#include "plugin.h"

using namespace std;

struct CarAttachments {
	CVehicle* vehicle;
	CVehicle* attached;
	CMatrix offset;
};

extern map<int, CarAttachments> carAttachments;

void attachVehicleToVehicle(int attachedHandle, int vehicleToAttachHandle);
void detachVehicleFromVehicle(int attachedHandle);
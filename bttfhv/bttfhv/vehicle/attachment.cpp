#include "attachment.h"

map<int, CarAttachments> carAttachments;

void attachVehicleToVehicle(int attachedHandle, int vehicleToAttachHandle) {
	CVehicle* attached = CPools::GetVehicle(attachedHandle);
	CVehicle* vehicle = CPools::GetVehicle(vehicleToAttachHandle);
	if (vehicle && attached) {
		carAttachments[attachedHandle].vehicle = vehicle;
		carAttachments[attachedHandle].attached = attached;
		carAttachments[attachedHandle].offset = Invert(vehicle->m_placement) * attached->m_placement;
	}
}

void detachVehicleFromVehicle(int attachedHandle) {
	if (carAttachments.contains(attachedHandle)) {
		carAttachments.erase(attachedHandle);
	}
}
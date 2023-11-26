#include "vehicle.h"

CVector getOffsetFromCar(CVehicle* vehicle, CVector offset) {
	return  Multiply3x3(vehicle->m_placement, offset) + vehicle->GetPosition();
}
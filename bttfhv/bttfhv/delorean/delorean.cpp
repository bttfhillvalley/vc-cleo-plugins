#include "../constants.h"
#include "../utils/math.h"
#include "../vehicle/components.h"

#include "components.h"
#include "delorean.h"

map<int, Delorean> deloreanMap;

Delorean::Delorean(int handle) {
	vehicleHandle = handle;
	CVehicle* vehicle = CPools::GetVehicle(handle);
	timeMachine = reinterpret_cast<CAutomobile*>(vehicle);
}

void Delorean::AnimateDoorStruts() {
	float leftDoorAngle = abs(timeMachine->m_aDoors[DOOR_FRONT_LEFT].fAngle);
	float leftPiston = sqrtf(SQR(DOOR_PIVOT) + SQR(BODY_PIVOT) - (2.0f * DOOR_PIVOT * BODY_PIVOT * cos(leftDoorAngle)));
	float leftStrutAngle = 86.6f - degrees(asinf(sinf(leftDoorAngle) * DOOR_PIVOT / leftPiston));
	float rightDoorAngle = abs(timeMachine->m_aDoors[DOOR_FRONT_RIGHT].fAngle);
	float rightPiston = sqrtf(SQR(DOOR_PIVOT) + SQR(BODY_PIVOT) - (2.0f * DOOR_PIVOT * BODY_PIVOT * cos(rightDoorAngle)));
	float rightStrutAngle = -(86.6f - degrees(asinf(sinf(rightDoorAngle) * DOOR_PIVOT / rightPiston)));

	rotateComponent(timeMachine, "doorlfstrut_", 0.0f, leftStrutAngle, 0.0f);
	rotateComponent(timeMachine, "doorrfstrut_", 0.0f, rightStrutAngle, 0.0f);
	moveComponent(timeMachine, "doorlfstrutp", 0.0f, 0.0f, leftPiston - 0.259f);
	moveComponent(timeMachine, "doorrfstrutp", 0.0f, 0.0f, rightPiston - 0.259f);
}
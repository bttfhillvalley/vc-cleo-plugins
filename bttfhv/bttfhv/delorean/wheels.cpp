#include "../constants.h"
#include "../vehicle/components.h"
#include "../vehicle.h"
#include "../utils/math.h"
#include "delorean.h"
#include <iostream>

#define WHEEL_FRONT_OFFSET (-0.280126f)
#define WHEEL_REAR_OFFSET (-0.304341f)

void Delorean::ProcessWheels() {
	CVector extension = getComponentPosition(automobile, "strutunitrb_");
	CVector rotation = getComponentRotation(automobile, "hoverjointrb_");
	if (abs(extension.x) < 0.1f) {
		// Use GTA native wheel speed/rotation
		copy(begin(automobile->fWheelRot), end(automobile->fWheelRot), begin(wheelRot));
		copy(begin(automobile->fWheelSpeed), end(automobile->fWheelSpeed), begin(wheelSpeed));
	}
	else {
		// Slow wheel and disconnect from GTA motion
		for (int n = 0; n < 4; n++) {
			if (automobile->fWheelSuspDist[n] < 1.0f) {
				wheelRot[n] = automobile->fWheelRot[n];
				wheelSpeed[n] = automobile->fWheelSpeed[n];
			}
			else {
				wheelSpeed[n] *= 0.90f;
				wheelRot[n] += wheelSpeed[n];
				automobile->fWheelRot[n] = wheelRot[n];
				automobile->fWheelSpeed[n] = wheelSpeed[n];
			}
		}
	}
	// Damper for hover
	float damper = extension.x / -0.3f + 1.0f;
	float steeringDamper = rotation.y / 90.0f + 1.0f;

	// Steering
	CVector steering(0.0f, 0.0f, degrees(automobile->m_fSteerAngle * 16.0f / 30.0f * steeringDamper));
	rotateComponent(automobile, "holderlf_", steering);
	rotateComponent(automobile, "holderrf_", steering);

	for (int n = 0; n < 4; n++) {
		// Suspension
		string w;
		switch (n) {
		case WHEEL_FRONT_LEFT:
			w = "lf";
			break;
		case WHEEL_REAR_LEFT:
			w = "lb";
			break;
		case WHEEL_FRONT_RIGHT:
			w = "rf";
			break;
		case WHEEL_REAR_RIGHT:
			w = "rb";
			break;
		}
		float ext = w[0] == 'l' ? abs(extension.x) * -1.0f : abs(extension.x);
		float offset = w[1] == 'f' ? WHEEL_FRONT_OFFSET : WHEEL_REAR_OFFSET;
		CVector pos = getComponentPosition(automobile, "wheel_" + w + "_dummy");
		CVector unit(ext, 0.0f, 0.0f);
		CVector arm(0.0f, 0.0f, (pos.z - offset) * damper);

		moveComponent(automobile, "strutunit" + w + "_", unit);
		moveComponent(automobile, "strutarm" + w + "_", arm);

		// Rotation
		CVector rotation(degrees(wheelRot[n]), 0.0f, 0.0f);
		rotateComponent(automobile, "fxwheel" + w + "_", rotation);

		// Damaged Wheels
		unsigned int wheelStatus = automobile->m_carDamage.GetWheelStatus(n);
		if (wheelStatus == WHEEL_STATUS_MISSING) continue;
		for (int t = 1; t <= 3; t++) {
			if (t == 2) continue;
			string wheel = "fxwheelbttf" + to_string(t) + w;
			string tire = "fxtirebttf" + to_string(t) + w;
			if (getVisibility(automobile, wheel)) {
				if (getVisibility(automobile, tire) && wheelStatus == WHEEL_STATUS_BURST) {
					CVector pos(0.0f, 0.0f, 0.0f);
					switch (n) {
					case WHEEL_FRONT_LEFT:
						automobile->GetComponentWorldPosition(CAR_WHEEL_LF, pos);
						break;
					case WHEEL_REAR_LEFT:
						automobile->GetComponentWorldPosition(CAR_WHEEL_LB, pos);
						break;
					case WHEEL_FRONT_RIGHT:
						automobile->GetComponentWorldPosition(CAR_WHEEL_RF, pos);
						break;
					case WHEEL_REAR_RIGHT:
						automobile->GetComponentWorldPosition(CAR_WHEEL_RB, pos);
						break;
					}
					unsigned char oldColor = automobile->m_nPrimaryColor;
					automobile->m_nPrimaryColor = 0;
					automobile->dmgDrawCarCollidingParticles(pos, 500.0f);
					automobile->m_nPrimaryColor = oldColor;
					setVisibility(automobile, tire, 0);
				}
				else if (!getVisibility(automobile, tire) && wheelStatus == WHEEL_STATUS_OK) {
					setVisibility(automobile, tire, 1);
				}
				break;
			}
		}
	}
}
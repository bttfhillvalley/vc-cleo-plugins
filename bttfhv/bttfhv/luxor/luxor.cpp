#include "CDamageManager.h"
#include "CVehicleModelInfo.h"
#include "CWorld.h"
#include "../constants.h"
#include "../utils/math.h"
#include "../sound/sound.h"
#include "../vehicle/components.h"
#include "../vehicle/handling.h"
#include "../vehicle/hover.h"
#include <iostream>

#include "luxor.h"

map<CVehicle*, Luxor*> luxorMap;

Luxor::Luxor(CVehicle* vehicle) {
	automobile = reinterpret_cast<CAutomobile*>(vehicle);
	flying = STATUS_LANDED;
	flyingFrame = 0;
	keyDown = false;
	wheelsDown = true;
	setVisibility(automobile, "strutlb", 1);
	setVisibility(automobile, "strutrb", 1);
	setVisibility(automobile, "strutlf", 1);
	setVisibility(automobile, "strutrf", 1);
	UpdateFlyingHandling(automobile, "delorean");
}

void Luxor::Setup() {
}

void Luxor::moveSuspension(string wheel, float offset) {
	float damper = getHoverDamper();
	CVector wheelPosition = getComponentPosition(automobile, "wheel_" + wheel + "_dummy");
	wheelPosition.x = 0.0f;
	wheelPosition.y = 0.0f;
	wheelPosition.z = damper * (wheelPosition.z - offset);
	moveComponent(automobile, "strutunit" + wheel + "_", wheelPosition);
}

float Luxor::getHoverDamper() {
	float damper = static_cast<float>(flyingFrame);
	damper = clamp(damper / -90.0f + 1.0f, 0.0f, 1.0f);
	return damper;
}

void Luxor::UpdateWheels() {
	CVector wheelPosition;
	float damper = getHoverDamper();
	if (flying == STATUS_LANDED) {
		for (int n = 0; n < 4; n++) {
			wheelRot[n] = degrees(automobile->fWheelRot[n]);
			wheelSpeed[n] = automobile->fWheelSpeed[n];
		}
	}
	else {
		for (int n = 0; n < 4; n++) {
			if (wheelSpeed[n] >= 0.05f) {
				wheelSpeed[n] -= 0.1f;
			}
			else if(wheelSpeed[n] <= -0.05f) {
				wheelSpeed[n] += 0.1f;
			}
			else {
				wheelSpeed[n] = 0.0f;
			}
			wheelRot[n] += degrees(wheelSpeed[n]);
			automobile->fWheelRot[n] = radians(wheelRot[n]);
			automobile->fWheelSpeed[n] = wheelSpeed[n];
		}
	}

	if (flyingFrame < 30) {
		if (wheelsDown) {
			for (int n = 0; n < 4; n++) {
				wheelStatus[n] = automobile->m_carDamage.GetWheelStatus(n);
			}
		}
		else {
			wheelsDown = true;
			for (int n = 0; n < 4; n++) {
				automobile->m_carDamage.SetWheelStatus(n, wheelStatus[n]);
			}
		}
	}
	else {
		if (wheelsDown) {
			wheelsDown = false;
			for (int n = 0; n < 4; n++) {
				wheelStatus[n] = automobile->m_carDamage.GetWheelStatus(n);
				automobile->m_carDamage.SetWheelStatus(n, 2);
			}
		}
	}

	// Move suspension
	moveSuspension("lf", LUXOR_FRONT_OFFSET);
	moveSuspension("lb", LUXOR_BACK_OFFSET);
	moveSuspension("rf", LUXOR_FRONT_OFFSET);
	moveSuspension("rb", LUXOR_BACK_OFFSET);

	// Rotate Wheels
	rotateComponent(automobile, "fxwheellf_", CVector(wheelRot[0], 0.0f, 0.0f));
	rotateComponent(automobile, "fxwheellb_", CVector(wheelRot[1], 0.0f, 0.0f));
	rotateComponent(automobile, "fxwheelrf_", CVector(wheelRot[2], 0.0f, 0.0f));
	rotateComponent(automobile, "fxwheelrb_", CVector(wheelRot[3], 0.0f, 0.0f));

	// Steer Wheels
	float steeringAngle = degrees(automobile->m_fSteerAngle) * 0.6f * damper;
	rotateComponent(automobile, "holderlf_", CVector(0.0f, 0.0f, steeringAngle));
	rotateComponent(automobile, "holderrf_", CVector(0.0f, 0.0f, steeringAngle));
}

void Luxor::HandleHover() {
	char filename[64];
	if (CPad::GetPad(0)->DisablePlayerControls || !isPlayerInCar(automobile)) {
		return;
	}
	if (CPad::GetPad(0)->GetLookBehindForPed() && !keyDown) {
		keyDown = true;
		if (flying == STATUS_LANDED || flying == STATUS_LANDING) {
			flying = STATUS_TAKING_OFF;
			sprintf(filename, "delorean/hover_extend.wav");
			int index = findEmptyIndex(filename);
			string key = getKeyIndex(filename, index);
			attachSoundFileToVehicle(automobile, key, filename, 0, 0.0f, 0.0f, 0.0f, 10.0f);
		}
		else {
			flying = STATUS_LANDING;
			sprintf(filename, "delorean/hover_retract.wav");
			int index = findEmptyIndex(filename);
			string key = getKeyIndex(filename, index);
			attachSoundFileToVehicle(automobile, key, filename, 0, 0.0f, 0.0f, 0.0f, 10.0f);
		}
	}
	else if (!CPad::GetPad(0)->GetLookBehindForPed() && keyDown) {
		keyDown = false;
	}
}

void Luxor::UpdateHover() {
	if (!getVisibility(automobile, "strutlb")) {
		setVisibility(automobile, "strutlb", 1);
		setVisibility(automobile, "strutrb", 1);
		setVisibility(automobile, "strutlf", 1);
		setVisibility(automobile, "strutrf", 1);
	}
	if (flying == STATUS_TAKING_OFF) {
		if (++flyingFrame == 90) {
			flying = STATUS_FLYING;
		}
	}
	else if (flying == STATUS_LANDING) {
		if (flyingFrame > 0) {
			--flyingFrame;
		}
		if (flyingFrame == 0 && automobile->nWheelsOnGround > 0) {
			flying = STATUS_LANDED;
		}
	}

	if (flying != STATUS_LANDED) {
		HoverControl(automobile, false, flying == STATUS_LANDING, false);
	}
	float strutAngle = static_cast<float>(flyingFrame);
	float rearWheelAngle = static_cast<float>(flyingFrame);
	float frontWheelAngle = static_cast<float>(flyingFrame);
	if (flyingFrame < 30) {
		strutAngle *= 2.0f;
		rearWheelAngle = 0.0f;
		frontWheelAngle = 0.0f;
	}
	else if (flyingFrame < 60) {
		strutAngle = 60.0f;
		rearWheelAngle = (rearWheelAngle - 30.0f) * 3.0f;
		frontWheelAngle = 0.0f;
	}
	else {
		strutAngle = -2.0f * (strutAngle - 60.0f) + 60.0f;
		rearWheelAngle = 90.0f;
		frontWheelAngle = (frontWheelAngle - 60.0f) * 3.0f;
	}

	rotateComponent(automobile, "strutarmlb_", CVector(strutAngle, 0.0f, 0.0f));
	rotateComponent(automobile, "strutarmrb_", CVector(strutAngle, 0.0f, 0.0f));
	rotateComponent(automobile, "hoverjointlb_", CVector(0.0f, -rearWheelAngle, 0.0f));
	rotateComponent(automobile, "hoverjointrb_", CVector(0.0f, rearWheelAngle, 0.0f));
	rotateComponent(automobile, "strutarmlf_", CVector(0.0f, -frontWheelAngle, 0.0f));
	rotateComponent(automobile, "strutarmrf_", CVector(0.0f, frontWheelAngle, 0.0f));
}

void Luxor::Update() {
	HandleHover();

	UpdateWheels();
	UpdateHover();
}
#include "CWorld.h"
#include "../constants.h"
#include "../utils/math.h"
#include "../player.h"
#include "../sound/sound.h"
#include "../vehicle/components.h"
#include "../vehicle.h"
#include "delorean.h"

#define BODY_PIVOT (0.364084f)
#define DOOR_PIVOT (0.105361f)
#define BONNET_PIVOT_LENGTH (0.197986f)
#define STRUT_LENGTH_OFFSET (0.548622f)
#define BONNET_ANGLE_OFFSET radians(5.512478f)
#define BONNET_STRUT_PIVOT CVector(-0.643365f, 1.90636f, 0.131049f)
#define BONNET_PIVOT CVector(0.0f, 2.10343f, 0.11203f)
#define STOCK_BONNET_STRUT CVector(-0.715233f, 1.36499f, 0.078711f)
#define HOVER_BONNET_STRUT CVector(-0.712656f, 1.50014f, 0.080155f)

void Delorean::ProcessDoor() {
	// Doors
	if (automobile->m_carDamage.GetDoorStatus(DOOR_FRONT_LEFT) != 2) {
		rotateComponent(automobile, "fxdoorlf_", -getComponentRotation(automobile, "door_lf_dummy"));
		float leftDoorAngle = abs(automobile->m_aDoors[DOOR_FRONT_LEFT].fAngle);
		float leftPiston = sqrtf(SQR(DOOR_PIVOT) + SQR(BODY_PIVOT) - (2.0f * DOOR_PIVOT * BODY_PIVOT * cos(leftDoorAngle)));
		float leftStrutAngle = 86.6f - degrees(asinf(sinf(leftDoorAngle) * DOOR_PIVOT / leftPiston));
		rotateComponent(automobile, "doorlfstrut_", 0.0f, leftStrutAngle, 0.0f);
		moveComponent(automobile, "doorlfstrutp", 0.0f, 0.0f, leftPiston - 0.259f);
	}

	if (automobile->m_carDamage.GetDoorStatus(DOOR_FRONT_RIGHT) != 2) {
		rotateComponent(automobile, "fxdoorrf_", -getComponentRotation(automobile, "door_rf_dummy"));
		float rightDoorAngle = abs(automobile->m_aDoors[DOOR_FRONT_RIGHT].fAngle);
		float rightPiston = sqrtf(SQR(DOOR_PIVOT) + SQR(BODY_PIVOT) - (2.0f * DOOR_PIVOT * BODY_PIVOT * cos(rightDoorAngle)));
		float rightStrutAngle = -(86.6f - degrees(asinf(sinf(rightDoorAngle) * DOOR_PIVOT / rightPiston)));
		rotateComponent(automobile, "doorrfstrut_", 0.0f, rightStrutAngle, 0.0f);
		moveComponent(automobile, "doorrfstrutp", 0.0f, 0.0f, rightPiston - 0.259f);
	}
}

void Delorean::ProcessBonnet() {
	if (automobile->m_carDamage.GetDoorStatus(BONNET) == 2) return;
	rotateComponent(automobile, "fxbonnet_", getComponentRotation(automobile, "bonnet_dummy"));
	float hoodAngle = -automobile->m_aDoors[BONNET].fAngle + BONNET_ANGLE_OFFSET;
	CVector attachment;
	if (getVisibility(automobile, "bonnetanchorl")) {
		attachment = STOCK_BONNET_STRUT;
	}
	else {
		attachment = HOVER_BONNET_STRUT;
	}

	CVector hoodStrutPivot = CVector(
		0.0f,
		-BONNET_PIVOT_LENGTH * cosf(hoodAngle) + BONNET_PIVOT.y,
		BONNET_PIVOT_LENGTH * sinf(hoodAngle) + BONNET_PIVOT.z
	);
	CVector deltaAngle = CVector(
		BONNET_STRUT_PIVOT.x - attachment.x,
		hoodStrutPivot.y - attachment.y,
		hoodStrutPivot.z - attachment.z
	);
	CVector strutAngle = CVector(
		degrees(-atanf(deltaAngle.y / deltaAngle.z)),
		0.0f,
		degrees(-atanf(deltaAngle.x / deltaAngle.y))
	);
	float strutLength = deltaAngle.Magnitude() - STRUT_LENGTH_OFFSET;
	moveComponent(automobile, "bonnetlstrut_", attachment.x, attachment.y, attachment.z);
	moveComponent(automobile, "bonnetrstrut_", -attachment.x, attachment.y, attachment.z);
	rotateComponent(automobile, "bonnetlstrut_", strutAngle.x, strutAngle.y, strutAngle.z);
	rotateComponent(automobile, "bonnetrstrut_", strutAngle.x, strutAngle.y, -strutAngle.z);
	moveComponent(automobile, "bonnetlstrutp_", 0.0f, 0.0f, strutLength);
	moveComponent(automobile, "bonnetrstrutp_", 0.0f, 0.0f, strutLength);
}

void Delorean::HandleBonnet() {
	if (automobile->IsDoorMissing(CAR_BONNET)) {
		if (!automobile->IsDoorClosed(BONNET)) {
			automobile->OpenDoor(CAR_BONNET, BONNET, 0.0f);
			bonnetState = BONNET_CLOSED;
		}
		return;
	}
	CPlayerInfo* playerInfo = &CWorld::Players[CWorld::PlayerInFocus];
	if (playerInfo->m_nPlayerState != PLAYERSTATE_PLAYING) {
		return;
	}
	CVector front = getOffsetFromCar(automobile, CVector(0.0f, 3.25f, 0.0f));
	CVector left = getOffsetFromCar(automobile, CVector(-1.5f, 1.5f, 0.0f));
	CVector right = getOffsetFromCar(automobile, CVector(1.5f, 1.5f, 0.0f));
	if (CPad::GetPad(0)->NewState.LeftShoulder1 != 0
		&& (isPlayerInSphere(front, CVector(0.75f, 0.75f, 2.0f))
		|| isPlayerInSphere(left, CVector(0.75f, 0.75f, 2.0f))
		|| isPlayerInSphere(right, CVector(0.75f, 0.75f, 2.0f)))) {
		switch (bonnetState) {
		case BONNET_OPENING:
		case BONNET_CLOSING:
			break;
		case BONNET_OPEN:
			bonnetState = BONNET_CLOSING;
			break;
		case BONNET_CLOSED:
			bonnetState = BONNET_OPENING;
		}
	}
	float doorAngle;
	CVector relativeVelocity;
	switch (bonnetState) {
	case BONNET_OPEN:
		relativeVelocity = Multiply3x3(automobile->m_vecMoveSpeed, automobile->m_placement);
		if (relativeVelocity.y * GAME_SPEED_TO_METERS_PER_SECOND > 30.0f) {
			GetSpeed();
			bonnetState = BONNET_CLOSING;
		}
		break;
	case BONNET_CLOSED:
		break;
	case BONNET_OPENING:
		doorAngle = Clamp(automobile->m_aDoors[BONNET].GetAngleOpenRatio() + 0.05f, 0.0f, 1.0f);
		automobile->OpenDoor(CAR_BONNET, BONNET, doorAngle);
		if (doorAngle >= 1.0f) {
			bonnetState = BONNET_OPEN;
		}
		break;
	case BONNET_CLOSING:
		doorAngle = Clamp(automobile->m_aDoors[BONNET].GetAngleOpenRatio() - 0.05f, 0.0f, 1.0f);
		automobile->OpenDoor(CAR_BONNET, BONNET, doorAngle);
		if (doorAngle <= 0.0f) {
			bonnetState = BONNET_CLOSED;
		}
		break;
	}
}

void Delorean::HandleBoot() {
	rotateComponent(automobile, "fxboot_", getComponentRotation(automobile, "boot_dummy"));
	if (automobile->IsDoorMissing(CAR_BOOT) || IsTimeMachine()) {
		if (!automobile->IsDoorClosed(BOOT)) {
			automobile->OpenDoor(CAR_BOOT, BOOT, 0.0f);
			rotateComponent(automobile, "enginecover", CVector(0.0f, 0.0f, 0.0f));
			bootState = BOOT_CLOSED;
		}
		return;
	}
	CPlayerInfo* playerInfo = &CWorld::Players[CWorld::PlayerInFocus];
	if (playerInfo->m_nPlayerState != PLAYERSTATE_PLAYING) {
		return;
	}
	CVector pos = getOffsetFromCar(automobile, CVector(0.0f, -3.25f, 0.0f));
	if (CPad::GetPad(0)->NewState.LeftShoulder1 != 0 && isPlayerInSphere(pos, CVector(0.75f, 0.75f, 2.0f))) {
		switch (bootState) {
		case BOOT_OPENING:
		case BOOT_CLOSING:
			break;
		case BOOT_OPEN:
			bootState = BOOT_CLOSING;
			break;
		case BOOT_CLOSED:
			bootState = BOOT_OPENING;
		}
	}
	float doorAngle;
	CVector coverAngle;

	switch (bootState) {
	case BOOT_OPEN:
	case BOOT_CLOSED:
		break;
	case BOOT_OPENING:
		doorAngle = Clamp(automobile->m_aDoors[BOOT].GetAngleOpenRatio() + 0.05f, 0.0f, 1.0f);
		automobile->OpenDoor(CAR_BOOT, BOOT, doorAngle);
		if (doorAngle >= 1.0f) {
			coverAngle = getComponentRotation(automobile, "enginecover");
			coverAngle.x = Clamp(coverAngle.x - 3.0f, -54.0f, 0.0f);
			rotateComponent(automobile, "enginecover", coverAngle);
			if (coverAngle.x <= -54.0f) {
				bootState = BOOT_OPEN;
			}
		}
		break;
	case BOOT_CLOSING:
		doorAngle = automobile->m_aDoors[BOOT].GetAngleOpenRatio();
		if (doorAngle <= 0.0f) {
			bootState = BOOT_CLOSED;
		}
		else {
			coverAngle = getComponentRotation(automobile, "enginecover");
			coverAngle.x = Clamp(coverAngle.x + 3.0f, -54.0f, 0.0f);
			rotateComponent(automobile, "enginecover", coverAngle);
			if (coverAngle.x >= 0.0f) {
				doorAngle = Clamp(doorAngle - 0.05f, 0.0f, 1.0f);
				automobile->OpenDoor(CAR_BOOT, BOOT, doorAngle);
			}
		}
		break;
	}
}
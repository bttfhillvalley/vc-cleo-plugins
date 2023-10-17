#define _USE_MATH_DEFINES
#include <cmath>
#include "CModelInfo.h"
#include "CParticle.h"
#include "CWorld.h"
#include "eEntityStatus.h"

#include "../constants.h"
#include "../configuration/fileloader.h"
#include "../utils/math.h"
#include "../vehicle/attachment.h"
#include "../vehicle/components.h"
#include "../vehicle/handling.h"
#include "../vehicle/hover.h"

#include "opcodes.h"

bool isPlayerInCar(CVehicle* vehicle) {
	CPlayerInfo player = CWorld::Players[CWorld::PlayerInFocus];
	if (player.IsPlayerInRemoteMode()) {
		return vehicle->m_nState == STATUS_PLAYER_REMOTE;
	}
	return player.m_pPed->m_bInVehicle && player.m_pPed->m_pVehicle == vehicle;
}

bool isPlayerInModel(int model) {
	CPlayerInfo player = CWorld::Players[CWorld::PlayerInFocus];
	return player.m_pPed->m_bInVehicle && player.m_pPed->m_pVehicle->m_nModelIndex == model;
}

eOpcodeResult __stdcall raiseFrontSuspension(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	int raise = Params[1].nVar;
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		if (!raise) {
			vehicle->m_nVehicleFlags.bUsePlayerColModel = false;
			automobile->SetupSuspensionLines();
			return OR_CONTINUE;
		}
		CPlayerInfo* playerInfo = &CWorld::Players[CWorld::PlayerInFocus];

		CVehicleModelInfo* mi = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));
		CColModel* colModel = mi->GetColModel();
		int i;
		float wheelRadius = 0.5f * mi->m_fWheelSize;
		float normalUpperLimit = automobile->m_pHandlingData->fSuspUpperLimit;
		float normalLowerLimit = automobile->m_pHandlingData->fSuspLowerLimit;
		float normalSpringLength = normalUpperLimit - normalLowerLimit;
		float normalLineLength = normalSpringLength + wheelRadius;

		float extendedUpperLimit = normalUpperLimit - 0.15f;
		float extendedLowerLimit = normalLowerLimit - 0.15f;
		float extendedSpringLength = extendedUpperLimit - extendedLowerLimit;

		// Update collision model
		playerInfo->m_ColModel = *colModel;
		CColModel* specialColModel = &playerInfo->m_ColModel;
		vehicle->m_nVehicleFlags.bUsePlayerColModel = true;

		CVector pos;
		for (i = 0; i < 4; i++) {
			mi->GetWheelPosn(i, pos);
			automobile->fWheelPos[i] = pos.z;

			// uppermost wheel position
			if (i % 2) {

				pos.z += normalUpperLimit;
			}
			else {
				pos.z += extendedUpperLimit;
			}
			specialColModel->m_pLines[i].m_vStart = pos;

			// lowermost wheel position
			pos.z -= normalLineLength;
			specialColModel->m_pLines[i].m_vEnd = pos;

			if (i % 2) {
				automobile->fSuspSpringLength[i] = normalSpringLength;
			}
			else {
				automobile->fSuspSpringLength[i] = extendedSpringLength;
			}
			automobile->fSuspLineLength[i] = specialColModel->m_pLines[i].m_vStart.z - specialColModel->m_pLines[i].m_vEnd.z;
		}
		automobile->fWheelAngleMul = 0.75f;
		// Adjust col model to include suspension lines
		mi->GetWheelPosn(0, pos);
		float minz = pos.z + extendedLowerLimit - wheelRadius;
		if (minz < colModel->m_boundBox.m_vecMin.z)
			colModel->m_boundBox.m_vecMin.z = minz;
		float radius = max(colModel->m_boundBox.m_vecMin.Magnitude(), colModel->m_boundBox.m_vecMax.Magnitude());
		if (colModel->m_colSphere.m_fRadius < radius)
			colModel->m_colSphere.m_fRadius = radius;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall updateHandling(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		UpdateHandling(vehicle);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall turnOnEngine(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		vehicle->m_nState = 0;
		vehicle->m_nVehicleFlags.bEngineOn = 1;
	}

	return OR_CONTINUE;
}

eOpcodeResult __stdcall getEngineStatus(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	int status = 0;
	if (vehicle) {
		status = vehicle->m_nState == 0 || vehicle->m_nState == 10;
	}
	Params[0].nVar = status;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCurrentGear(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		Params[0].nVar = (int)vehicle->m_nCurrentGear;
		script->Store(1);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setHover(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	bool landing = Params[1].nVar != 0;
	bool damage = Params[2].nVar != 0;
	if (vehicle) {
		HoverControl(vehicle, landing, damage);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isDoorOpen(CScript* script) {
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {

		Params[0].nVar = (int)vehicle->IsDoorClosed(static_cast<eDoors>(Params[1].nVar));
		script->Store(1);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getDoorAngle(CScript* script) {
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		Params[0].fVar = degrees(automobile->m_aDoors[Params[1].nVar].fAngle);
		script->Store(1);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getWheelAngle(CScript* script) {
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int n = 0; n < 4; n++) {
			Params[n].fVar = degrees(automobile->fWheelRot[n]);
		}
		script->Store(4);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setWheelAngle(CScript* script) {
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int n = 0; n < 4; n++) {
			automobile->fWheelRot[n] = radians(Params[n + 1].fVar);
		}
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getWheelSpeed(CScript* script) {
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int n = 0; n < 4; n++) {
			Params[n].fVar = automobile->fWheelSpeed[n];
		}
		script->Store(4);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setWheelSpeed(CScript* script) {
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int n = 0; n < 4; n++) {
			automobile->fWheelSpeed[n] = Params[n + 1].fVar;
		}
	}
	return OR_CONTINUE;
}


eOpcodeResult __stdcall getDriveWheelsOnGround(CScript* script) {
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		Params[0].nVar = (int)automobile->nRearWheelsOnGround;
		script->Store(1);
	}
	return OR_CONTINUE;
}

bool wheelsOnGround(int vehiclePointer) {
	CVehicle* vehicle = CPools::GetVehicle(vehiclePointer);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		return automobile->nWheelsOnGround > 0;
	}
	return false;
}

eOpcodeResult __stdcall isWheelsOnGround(CScript* script) {
	script->Collect(1);
	script->UpdateCompareFlag(wheelsOnGround(Params[0].nVar));
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isWheelsNotOnGround(CScript* script) {
	script->Collect(1);
	script->UpdateCompareFlag(!wheelsOnGround(Params[0].nVar));
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarStatus(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	vehicle->m_nState = Params[1].nVar & 0xFF;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarStatus(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	Params[0].nVar = vehicle->m_nState;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarLights(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	vehicle->m_nVehicleFlags.bLightsOn = Params[1].nVar != 0;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarLights(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	Params[0].nVar = vehicle->m_nVehicleFlags.bLightsOn;
	script->Store(1);
	return OR_CONTINUE;
}

// Opcodes
eOpcodeResult __stdcall setCarComponentVisibility(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	setVisibility(vehicle, Params[1].cVar, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentVisible(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	int visibility = getVisibility(vehicle, Params[1].cVar);
	script->UpdateCompareFlag(visibility != 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentNotVisible(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	int visibility = getVisibility(vehicle, Params[1].cVar);
	script->UpdateCompareFlag(visibility == 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentIndexVisible(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	int visibility = getVisibility(vehicle, component);
	script->UpdateCompareFlag(visibility != 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentIndexNotVisible(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	int visibility = getVisibility(vehicle, component);
	script->UpdateCompareFlag(visibility == 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentGlow(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	setGlow(vehicle, Params[1].cVar, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentGlowIndex(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	setGlow(vehicle, component, Params[3].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentIndexVisibility(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	setVisibility(vehicle, component, Params[3].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentColor(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	setColor(vehicle, Params[1].cVar, Params[2].nVar, Params[3].nVar, Params[4].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentIndexColor(CScript* script)
{
	script->Collect(6);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	setColor(vehicle, component, Params[3].nVar, Params[4].nVar, Params[5].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentAlpha(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (isPlayerInCar(vehicle) || !isPlayerInModel(vehicle->m_nModelIndex)) {
		setAlpha(vehicle, Params[1].cVar, Params[2].nVar);
	}
	else {
		setVisibility(vehicle, Params[1].cVar, Params[2].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentIndexAlpha(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	if (isPlayerInCar(vehicle) || !isPlayerInModel(vehicle->m_nModelIndex)) {
		setAlpha(vehicle, component, Params[3].nVar);
	}
	else {
		setVisibility(vehicle, component, Params[3].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall fadeCarComponentAlpha(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (isPlayerInCar(vehicle) || !isPlayerInModel(vehicle->m_nModelIndex)) {
		fadeAlpha(vehicle, Params[1].cVar, Params[2].nVar, Params[3].nVar);
	}
	else {
		setVisibility(vehicle, Params[1].cVar, Params[2].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall fadeCarComponentIndexAlpha(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	if (isPlayerInCar(vehicle) || !isPlayerInModel(vehicle->m_nModelIndex)) {
		fadeAlpha(vehicle, component, Params[3].nVar, Params[4].nVar);
	}
	else {
		setVisibility(vehicle, component, Params[3].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall carComponentDigitOff(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	digitOff(vehicle, Params[1].cVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall carComponentDigitOn(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	digitOn(vehicle, Params[1].cVar, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarComponentAlpha(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	Params[0].nVar = getAlpha(vehicle, Params[1].cVar);
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarComponentIndexAlpha(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	Params[0].nVar = getAlpha(vehicle, component);
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall moveCarComponent(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	moveComponent(vehicle, Params[1].cVar, Params[2].fVar, Params[3].fVar, Params[4].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall moveCarComponentIndex(CScript* script)
{
	script->Collect(6);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	moveComponent(vehicle, component, Params[3].fVar, Params[4].fVar, Params[5].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarComponentPosition(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[1].cVar);
	if (frame) {
		CMatrix cmatrix(&frame->modelling, false);
		Params[0].fVar = cmatrix.pos.x;
		Params[1].fVar = cmatrix.pos.y;
		Params[2].fVar = cmatrix.pos.z;
	}
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarComponentOffset(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[1].cVar);
	if (frame) {
		CMatrix cmatrix(&frame->modelling, false);
		CVector offset = CVector(Params[2].fVar, Params[3].fVar, Params[4].fVar);
		CVector coords = Multiply3x3(vehicle->m_placement, Multiply3x3(cmatrix, offset) + cmatrix.pos) + vehicle->GetPosition();
		Params[0].fVar = coords.x;
		Params[1].fVar = coords.y;
		Params[2].fVar = coords.z;
	}
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateCarComponent(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	rotateComponent(vehicle, Params[1].cVar, Params[2].fVar, Params[3].fVar, Params[4].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateCarComponentIndex(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	rotateComponent(vehicle, Params[3].cVar, Params[4].fVar, Params[5].fVar, Params[6].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarComponentRotation(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[1].cVar);
	if (frame) {
		CMatrix cmatrix(&frame->modelling, false);
		Params[2].fVar = atan2f(cmatrix.right.y, cmatrix.up.y);
		if (Params[2].fVar < 0.0f)
			Params[2].fVar += (float)(2.0f * M_PI);
		float s = sinf(Params[2].fVar);
		float c = cosf(Params[2].fVar);
		Params[0].fVar = atan2f(-cmatrix.at.y, s * cmatrix.right.y + c * cmatrix.up.y);
		if (Params[0].fVar < 0.0f)
			Params[0].fVar += (float)(2.0f * M_PI);
		Params[1].fVar = atan2f(-(cmatrix.right.z * c - cmatrix.up.z * s), cmatrix.right.x * c - cmatrix.up.x * s);
		if (Params[1].fVar < 0.0f)
			Params[1].fVar += (float)(2.0f * M_PI);

		Params[0].fVar = degrees(Params[0].fVar);
		Params[1].fVar = degrees(Params[1].fVar);
		Params[2].fVar = degrees(Params[2].fVar);
	}
	script->Store(3);
	return OR_CONTINUE;
}


eOpcodeResult __stdcall rotateBonnet(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->OpenDoor(CAR_DOOR_LF, DOOR_FRONT_LEFT, Params[1].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateBoot(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->OpenDoor(CAR_BOOT, BOOT, Params[1].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getGasPedalAudio(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);;
		Params[0].nVar = automobile->nTireFriction[0];
		Params[1].nVar = automobile->nTireFriction[1];
		Params[2].nVar = automobile->nTireFriction[2];
		Params[3].nVar = automobile->nTireFriction[3];
	}
	script->Store(4);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarOrientation(CScript* script)
{
	script->Collect(1);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		vehicle->m_placement.GetOrientation(x, y, z);
	}
	if (isnan(x)) x = 0.0;
	if (isnan(y)) y = 0.0;
	if (isnan(z)) z = 0.0;
	Params[0].fVar = degrees(x);
	Params[1].fVar = degrees(y);
	Params[2].fVar = degrees(z);
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarOrientation(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		vehicle->m_placement.SetOrientation(radians(Params[0].fVar), radians(Params[1].fVar), radians(Params[2].fVar));
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall popWheelie(CScript* script)
{
	script->Collect(2);
	float x, y, z;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		Command<Commands::GET_CAR_HEADING>(vehicle, &z);
		z = radians(z);
		x = cos(z) * Params[1].fVar;
		y = sin(z) * Params[1].fVar;
		vehicle->m_vecTurnSpeed.x = x;
		vehicle->m_vecTurnSpeed.y = y;
		//vehicle->m_vecFrictionTurnForce.y = x;
		//vehicle->m_vecFrictionTurnForce.y = y;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall skiMode(CScript* script)
{
	script->Collect(2);
	float x, y, z;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		Command<Commands::GET_CAR_HEADING>(vehicle, &z);
		z += 90.0;
		z = radians(z);
		x = cos(z) * Params[1].fVar;
		y = sin(z) * Params[1].fVar;
		vehicle->m_vecTurnSpeed.x = x;
		vehicle->m_vecTurnSpeed.y = y;
		//vehicle->m_vecFrictionTurnForce.y = x;
		//vehicle->m_vecFrictionTurnForce.y = y;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateCar(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		vehicle->m_vecTurnSpeed.z = Params[1].fVar;
		//vehicle->m_vecFrictionTurnForce.y = x;
		//vehicle->m_vecFrictionTurnForce.y = y;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarRotation(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		Params[0].fVar = vehicle->m_vecTurnSpeed.z;
	}
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setRemote(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_nState = STATUS_PLAYER_REMOTE;  // Set remote mode
		automobile->m_nVehicleFlags.bEngineOn = 1;
		//info->m_pRemoteVehicle = vehicle;
		CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle = automobile;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall removeRemote(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_nState = STATUS_PHYSICS;  // Set remote mode
		automobile->m_nVehicleFlags.bEngineOn = 0;
	}
	CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle = NULL;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall inRemote(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CPlayerInfo player = CWorld::Players[CWorld::PlayerInFocus];
	static char  message[128];


	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		snprintf(message, 128, "Remote: %d, State: %d, this is a long string",
			player.IsPlayerInRemoteMode(),
			automobile->m_nState
		);
		//CMessages::AddMessageJumpQ(message, 150, 0);
		if (player.IsPlayerInRemoteMode() && automobile->m_nState == STATUS_PLAYER_REMOTE) {

			script->UpdateCompareFlag(1);
			return OR_CONTINUE;
		}
	}
	script->UpdateCompareFlag(0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall applyForwardForce(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CVector force = vehicle->m_placement.up * Params[1].fVar;
		vehicle->ApplyMoveForce(force);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall applyUpwardForce(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CVector force = vehicle->m_placement.at * Params[1].fVar;
		vehicle->ApplyMoveForce(force.x, force.y, force.z);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getWheelStatus(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	int nWheels = 4;
	unsigned char status = -1;
	if (vehicle) {
		status = 0;
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int i = 0; i < nWheels; i++) {
			status <<= 2;
			status |= automobile->m_carDamage.GetWheelStatus(i);
		}
	}
	Params[0].nVar = status;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setWheelStatus(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	int nWheels = 4;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		int status = Params[1].nVar;
		for (int i = nWheels - 1; i >= 0; i--) {
			int wStatus = status & 0b11;
			automobile->m_carDamage.SetWheelStatus(i, wStatus);
			status >>= 2;
		}
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall wheelSparks(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	float friction = Params[1].fVar;
	int ignoreLand = Params[3].nVar;
	CAutomobile* automobile;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int i = 0; i < 4; i++) {
			if (ignoreLand || automobile->fWheelSuspDist[i] < 1.0f) {
				static float speedSq;
				speedSq = automobile->m_vecMoveSpeed.Magnitude();
				if (speedSq > SQR(0.1f) && (ignoreLand || (
					automobile->stWheels[i].surfaceB != SURFACE_GRASS &&
					automobile->stWheels[i].surfaceB != SURFACE_MUD_DRY &&
					automobile->stWheels[i].surfaceB != SURFACE_SAND &&
					automobile->stWheels[i].surfaceB != SURFACE_SAND_BEACH &&
					automobile->stWheels[i].surfaceB != SURFACE_WATER))) {
					CVector normalSpeed = automobile->stWheels[i].vecWheelAngle * DotProduct(automobile->stWheels[i].vecWheelAngle, automobile->m_vecMoveSpeed);
					CVector frictionSpeed = automobile->m_vecMoveSpeed - normalSpeed;
					if (i == CARWHEEL_FRONT_LEFT || i == CARWHEEL_REAR_LEFT)
						frictionSpeed -= (1.0f - friction) / 15.0f * automobile->m_placement.right;
					else
						frictionSpeed += (1.0f - friction) / 15.0f * automobile->m_placement.right;
					CVector sparkDir = friction * frictionSpeed;
					CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->stWheels[i].vecWheelPos, sparkDir, NULL, 0.0f, 0, 0, 0, 0);

					if (speedSq > 0.04f)
						CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->stWheels[i].vecWheelPos, sparkDir, NULL, 0.0f, 0, 0, 0, 0);
					if (speedSq > 0.16f) {
						CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->stWheels[i].vecWheelPos, sparkDir, NULL, 0.0f, 0, 0, 0, 0);
						CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->stWheels[i].vecWheelPos, sparkDir, NULL, 0.0f, 0, 0, 0, 0);
					}
				}
			}
		}
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarCollision(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);


	if (vehicle) {
		//vehicle->m_nVehicleFlags.bEngineOn = Params[1].nVar;
		if (Params[1].nVar) {
			vehicle->RemoveFromMovingList();
			vehicle->m_nFlags.bUseCollision = true;
			vehicle->AddToMovingList();
		}
		else {
			vehicle->m_nFlags.bUseCollision = false;

		}
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarEngineSound(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);


	if (vehicle) {
		vehicle->m_nVehicleFlags.bEngineOn = Params[1].nVar;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getVehicleFlags(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	unsigned int flags = 0;
	if (vehicle) {
		flags = vehicle->m_pHandlingData->uFlags;
	}
	Params[0].nVar = flags;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setVehicleFlags(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		vehicle->m_pHandlingData->uFlags = Params[1].nVar;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getRotationMatrix(CScript* script)
{
	int forward = 0;
	int right = 0;
	int up = 0;
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		forward = convertMatrixToInt(vehicle->m_placement.up);
		right = convertMatrixToInt(vehicle->m_placement.right);
		up = convertMatrixToInt(vehicle->m_placement.at);
	}
	Params[0].nVar = forward;
	Params[1].nVar = right;
	Params[2].nVar = up;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setRotationMatrix(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		convertIntToMatrix(vehicle->m_placement.up, Params[1].nVar);
		convertIntToMatrix(vehicle->m_placement.right, Params[2].nVar);
		convertIntToMatrix(vehicle->m_placement.at, Params[3].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getUpMatrix(CScript* script)
{
	script->Collect(1);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		x = vehicle->m_placement.at.x;
		y = vehicle->m_placement.at.y;
		z = vehicle->m_placement.at.z;
	}
	Params[0].fVar = x;
	Params[1].fVar = y;
	Params[2].fVar = z;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getAtMatrix(CScript* script)
{
	script->Collect(1);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		x = vehicle->m_placement.up.x;
		y = vehicle->m_placement.up.y;
		z = vehicle->m_placement.up.z;
	}
	Params[0].fVar = x;
	Params[1].fVar = y;
	Params[2].fVar = z;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getRightMatrix(CScript* script)
{
	script->Collect(1);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		x = vehicle->m_placement.right.x;
		y = vehicle->m_placement.right.y;
		z = vehicle->m_placement.right.z;
	}
	Params[0].fVar = x;
	Params[1].fVar = y;
	Params[2].fVar = z;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getVelocityVector(CScript* script)
{
	int velocity = 0;
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		velocity = convertMatrixToInt(vehicle->m_vecMoveSpeed);
	}
	Params[0].nVar = velocity;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setVelocityVector(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		convertIntToMatrix(vehicle->m_vecMoveSpeed, Params[1].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getVelocityDirection(CScript* script)
{
	int velocity = 0;
	script->Collect(1);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		x = vehicle->m_vecMoveSpeed.x / vehicle->m_vecMoveSpeed.Magnitude();
		y = vehicle->m_vecMoveSpeed.y / vehicle->m_vecMoveSpeed.Magnitude();
		z = vehicle->m_vecMoveSpeed.z / vehicle->m_vecMoveSpeed.Magnitude();
	}
	Params[0].fVar = x;
	Params[1].fVar = y;
	Params[2].fVar = z;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getVelocity(CScript* script)
{
	int velocity = 0;
	script->Collect(1);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		x = vehicle->m_vecMoveSpeed.x;
		y = vehicle->m_vecMoveSpeed.y;
		z = vehicle->m_vecMoveSpeed.z;
	}
	Params[0].fVar = x;
	Params[1].fVar = y;
	Params[2].fVar = z;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setVelocity(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		vehicle->m_vecMoveSpeed.x = Params[1].fVar;
		vehicle->m_vecMoveSpeed.y = Params[2].fVar;
		vehicle->m_vecMoveSpeed.z = Params[3].fVar;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getForwardVelocityVectorWithSpeed(CScript* script)
{
	int forward = 0;
	int right = 0;
	int up = 0;
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		forward = convertMatrixToInt(vehicle->m_placement.up * (Params[1].fVar * 2.0f / 100.0f));
	}
	Params[0].nVar = forward;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getRelativeVelocity(CScript* script) {
	script->Collect(1);
	CVector relativeVelocity;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		relativeVelocity = Multiply3x3(vehicle->m_vecMoveSpeed, vehicle->m_placement);
	}
	Params[0].fVar = relativeVelocity.x;
	Params[1].fVar = relativeVelocity.y;
	Params[2].fVar = relativeVelocity.z;
	script->Store(3);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setGasPedal(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_fGasPedal = Clamp(Params[1].fVar, 0.0f, 1.0f);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setBrakePedal(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_fBreakPedal = Clamp(Params[1].fVar, 0.0f, 1.0f);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setHandBrake(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_nVehicleFlags.bIsHandbrakeOn = Params[1].nVar;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setSteeringAngle(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_fSteerAngle = radians(Clamp(Params[1].fVar, -automobile->m_pHandlingData->fSteeringLock, automobile->m_pHandlingData->fSteeringLock));
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getSteeringAngle(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	float angle = 0.0;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		angle = degrees(automobile->m_fSteerAngle);
	}
	Params[0].fVar = angle;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isAttached(CScript* script) {
	script->Collect(1);
	int index = Params[0].nVar;
	script->UpdateCompareFlag(carAttachments.contains(index));
	return OR_CONTINUE;
}

eOpcodeResult __stdcall attachVehicle(CScript* script) {
	script->Collect(2);
	int index = Params[0].nVar;
	attachVehicleToVehicle(Params[0].nVar, Params[1].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall detachVehicle(CScript* script) {
	script->Collect(1);
	detachVehicleFromVehicle(Params[0].nVar);
	return OR_CONTINUE;
}
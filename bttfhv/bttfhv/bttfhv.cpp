#define _USE_MATH_DEFINES
#include "plugin.h"
#include "irrKlang.h"
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <set>
#include "VC.CLEO.h"
#pragma comment(lib, "VC.CLEO.lib")
#include "CCamera.h"
#include "CClumpModelInfo.h"
#include "CFileLoader.h"
#include "CMenuManager.h"
#include "CModelInfo.h"
#include "CPointLights.h"
#include "CPools.h"
#include "CSimpleModelInfo.h"
#include "CTxdStore.h"
#include "CWorld.h"
#include "cHandlingDataMgr.h"
#include "extensions\ScriptCommands.h"

using namespace irrklang;
using namespace plugin;
using namespace std;

ISoundEngine* m_soundEngine;
char volume = 0;
int visibility;  // Hack for now, not sure how to pass this in as a (void*) without messing up the value
boolean paused = false;

struct GameSound {
	ISound* sound;
	CVehicle* vehicle;
	CVector offset{ 0.0,0.0,0.0 };
	bool spatial;
};

map<string, GameSound> soundMap;
map<string, set<int>> ideMap;
map<string, set<int>*> removeObjectQueue;

int &ms_atomicPluginOffset = *(int *)0x69A1C8;
//ofstream of("DEBUG", std::ofstream::app);

int __cdecl GetAtomicId(RpAtomic *atomic) {
	return *(&atomic->object.object.type + ms_atomicPluginOffset);
}

RwObject *__cdecl SetVehicleAtomicVisibilityCB(RwObject *rwObject, void *data) {
	if (data == (void *)(0))
		rwObject->flags = 0;
	else
		rwObject->flags = 4;
	return rwObject;
}

RwObject* __cdecl GetVehicleAtomicVisibilityCB(RwObject* rwObject, void* data) {
	visibility = (int)rwObject->flags;
	return rwObject;
}

RwObject *__cdecl GetVehicleAtomicObjectCB(RwObject* object, void* data)
{
	*(RpAtomic**)data = (RpAtomic*)object;
	return object;
}

float radians(float degrees) {
	return (float)(degrees * M_PI / 180.0);
}

float degrees(float radians) {
	return (float)(radians * 180.0 / M_PI);
}

tScriptVar *Params;
using namespace plugin;

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
		CColModel *specialColModel = &playerInfo->m_ColModel;
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
		status = vehicle->m_nVehicleFlags.bEngineOn;
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
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		if (Params[1].nVar == 1) {
			CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
			// TODO: Load per vehicle configuration from a file.
			vehicle->FlyingControl(2);
			vehicle->m_pFlyingHandling->fThrust = 0.50f;
			vehicle->m_pFlyingHandling->fThrustFallOff = 5.0f;
			vehicle->m_pFlyingHandling->fYaw = -0.001f;
			vehicle->m_pFlyingHandling->fYawStab = 0.0f;
			vehicle->m_pFlyingHandling->fSideSlip = 0.1f;
			vehicle->m_pFlyingHandling->fRoll = 0.0065f;
			vehicle->m_pFlyingHandling->fRollStab = 0.0f;
			vehicle->m_pFlyingHandling->fPitch = 0.0035f;
			vehicle->m_pFlyingHandling->fPitchStab = 0.0f;
			vehicle->m_pFlyingHandling->fFormLift = 0.0f;
			vehicle->m_pFlyingHandling->fAttackLift = 0.0f;
			vehicle->m_pFlyingHandling->fMoveRes = 0.997f;
			vehicle->m_pFlyingHandling->vecTurnRes.x = 0.9f;
			vehicle->m_pFlyingHandling->vecTurnRes.y = 0.9f;
			vehicle->m_pFlyingHandling->vecTurnRes.z = 0.99f;
			vehicle->m_pFlyingHandling->vecSpeedRes.x = 0.0f;
			vehicle->m_pFlyingHandling->vecSpeedRes.y = 0.0f;
			vehicle->m_pFlyingHandling->vecSpeedRes.z = 0.0f;

			CMatrix mat;
			CVector pos;

			/*//automobile->DoHoverSuspensionRatios();
			mat.Attach(RwFrameGetMatrix(automobile->m_aCarNodes[CAR_WHEEL_RB]), false);
			//mat.SetTranslate(mat.pos.x, mat.pos.y, mat.pos.z);
			mat.SetRotateY((float)(-M_PI / 2.0f));
			mat.Update();

			mat.Attach(RwFrameGetMatrix(automobile->m_aCarNodes[CAR_WHEEL_LB]), false);
			//mat.SetTranslate(mat.pos.x, mat.pos.y, mat.pos.z);
			mat.SetRotateY((float)(M_PI / 2.0f));
			mat.Update();

			mat.Attach(RwFrameGetMatrix(automobile->m_aCarNodes[CAR_WHEEL_RF]), false);
			//mat.SetTranslate(mat.pos.x, mat.pos.y, mat.pos.z);
			mat.SetRotateY((float)(-M_PI / 2.0f));
			mat.Update();

			mat.Attach(RwFrameGetMatrix(automobile->m_aCarNodes[CAR_WHEEL_LF]), false);
			//mat.SetTranslate(mat.pos.x, mat.pos.y, mat.pos.z);
			mat.SetRotateY((float)(M_PI / 2.0f));
			mat.Update();*/
			//testcar->Teleport(CVector(0.0, 0.0, 0.0));
		}
		else
		{
			vehicle->FlyingControl(1);
		}
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

// Car stuff
// Helper methods
void setVisibility(CVehicle* vehicle, char* component, int visibility) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RwFrameForAllObjects(frame, SetVehicleAtomicVisibilityCB, (void*)visibility);
	}
}

void getVisibility(CVehicle* vehicle, char* component) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RwFrameForAllObjects(frame, GetVehicleAtomicVisibilityCB, NULL);
	}
}

void moveComponent(CVehicle* vehicle, char* component, float x, float y, float z) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		CMatrix cmmatrix(&frame->modelling, false);
		cmmatrix.SetTranslateOnly(x, y, z);
		cmmatrix.UpdateRW();
	}
}

void setAlpha(CVehicle* vehicle, char* component, int alpha) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RpAtomic* atomic;
		RwFrameForAllObjects(frame, GetVehicleAtomicObjectCB, &atomic);
		vehicle->SetComponentAtomicAlpha(atomic, alpha);
	}
}

void rotateComponent(CVehicle* vehicle, char* component, float rx, float ry, float rz) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		CMatrix cmatrix(&frame->modelling, false);
		CVector cpos(cmatrix.pos);
		cmatrix.SetRotate(radians(rx), radians(ry), radians(rz));
		cmatrix.pos = cpos;
		cmatrix.UpdateRW();
	}
}

void setGlow(CVehicle* vehicle, char* component, int glow) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetVehicleAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		//geometry->matList.materials[0]->texture->name
		if (glow == 0) {
			geometry->flags &= 0xF7;  // Turn off Prelit
			geometry->flags |= 0x20;  // Turn on Light
		}
		else {
			geometry->flags |= 0x8;  // Turn on Prelit
			geometry->flags &= 0xDF;  // Turn off Light
		}
	}
}

// Opcodes
eOpcodeResult __stdcall setCarComponentVisibility(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	setVisibility(vehicle, Params[1].cVar, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getCarComponentVisibility(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	getVisibility(vehicle, Params[1].cVar);
	Params[0].nVar = visibility;
	script->Store(1);
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
	sprintf(component, "%s%d", Params[1].cVar, Params[3].nVar);
	setGlow(vehicle, component, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentIndexVisibility(CScript* script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[3].nVar);
	setVisibility(vehicle, component, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentAlpha(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	setAlpha(vehicle, Params[1].cVar, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentIndexAlpha(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[3].nVar);
	setAlpha(vehicle, component, Params[2].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall moveCarComponent(CScript *script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	moveComponent(vehicle, Params[0].cVar, Params[1].fVar, Params[2].fVar, Params[3].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall moveCarComponentIndex(CScript* script)
{
	script->Collect(6);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[0].cVar, Params[5].nVar);
	moveComponent(vehicle, component, Params[1].fVar, Params[2].fVar, Params[3].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateCarComponent(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	rotateComponent(vehicle, Params[0].cVar, Params[1].fVar, Params[2].fVar, Params[3].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateCarComponentIndex(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[0].cVar, Params[5].nVar);
	rotateComponent(vehicle, Params[0].cVar, Params[1].fVar, Params[2].fVar, Params[3].fVar);
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
	script->Collect(1);
	float x, y, z;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		Command<Commands::GET_CAR_HEADING>(vehicle, &z);
		z = radians(z);
		x = cos(z) * 0.25f;
		y = sin(z) * 0.25f;
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

eOpcodeResult __stdcall setRemote(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		automobile->m_nState = 10;  // Set remote mode
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
		automobile->m_nState = 4;  // Set remote mode
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
		if (player.IsPlayerInRemoteMode() && automobile->m_nState == 10) {

			script->UpdateCompareFlag(1);
			return OR_CONTINUE;
		}
	}
	script->UpdateCompareFlag(0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall applyForce(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CVector force = vehicle->m_placement.up * Params[1].fVar;
		vehicle->ApplyMoveForce(force.x, force.y, force.z);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getWheelStatus(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	int status = -1;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		status = automobile->m_carDamage.GetWheelStatus(0);
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
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		int status = Params[1].nVar;
		automobile->m_carDamage.SetWheelStatus(0, status);
		automobile->m_carDamage.SetWheelStatus(1, status);
		automobile->m_carDamage.SetWheelStatus(2, status);
		automobile->m_carDamage.SetWheelStatus(3, status);
		if (status == 0) {
			automobile->m_carDamage.uDamId = 0;
		}
	}
	return OR_CONTINUE;
}

int convertMatrixToInt(CVector vector) {
	return ((int)((vector.x + 4.0) * 100) * 1000000) + ((int)((vector.y + 4.0) * 100) * 1000) + ((int)((vector.z + 4.0) * 100));
}

void convertIntToMatrix(CVector& vector, int value) {
	int x, y, z;
	x = value / 1000000;
	y = (value - (x * 1000000)) / 1000;
	z = value - (x * 1000000) - (y * 1000);
	vector.x = x / 100.f - 4.0f;
	vector.y = y / 100.f - 4.0f;
	vector.z = z / 100.f - 4.0f;
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

eOpcodeResult __stdcall getVelocity(CScript* script)
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

set<int>* getModels(char* path) {
	string key(path);
	if (!ideMap.contains(key)) {
		ifstream in;
		char fullpath[128];
		char idepath[128];
		snprintf(fullpath, 128, "data\\%s.dat", path);
		plugin::config_file config(fullpath);
		for (auto param : config.paramLines) {
			// Only look up IDEs
			if (param.name == "IDE") {
				strncpy(idepath, param.asString().c_str(), 128);
				in.open(GAME_PATH(idepath));
				for (std::string line; getline(in, line); ) {
					// Find actual entries
					int end = line.find(",");
					if (end != std::string::npos) {
						ideMap[key].emplace(stoi(line.substr(0, end)));
					}
				}
				in.close();
			}
		}
	}
	return &ideMap[key];
}

eOpcodeResult __stdcall createLight(CScript* script)
{
	script->Collect(12);
	CVector point = CVector(Params[1].fVar, Params[2].fVar, Params[3].fVar);
	CVector direction = CVector(Params[4].fVar, Params[5].fVar, Params[6].fVar);
	CPointLights::AddLight(Params[0].nVar, point, direction, Params[7].fVar, Params[8].fVar, Params[9].fVar, Params[10].fVar, Params[11].nVar, false);
	return OR_CONTINUE;
}

// Building stuff
eOpcodeResult __stdcall addBuilding(CScript* script)
{
	script->Collect(1);
	set<int> *models;
	models = getModels(Params[0].cVar);
	for (auto object : CPools::ms_pBuildingPool) {
		if (models->find(object->m_nModelIndex) != models->end()) {
			CWorld::Add(object);
		}
	}
	/*for (auto object : CPools::ms_pDummyPool) {
		if (models.find(object->m_nModelIndex) != models.end()) {
			CWorld::Add(object);
		}
	}*/
	return OR_CONTINUE;
}

eOpcodeResult __stdcall removeBuilding(CScript* script)
{
	script->Collect(1);
	set<int> *models;
	models = getModels(Params[0].cVar);
	for (auto object : CPools::ms_pBuildingPool) {
		if (models->find(object->m_nModelIndex) != models->end()) {
			//of << object->m_nModelIndex << "," << (int)object->m_nType << std::endl;
			CWorld::Remove(object);
		}
	}

	return OR_CONTINUE;
}

eOpcodeResult __stdcall addObjects(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	if (removeObjectQueue.contains(key)) {
		removeObjectQueue.erase(key);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall removeObjects(CScript* script)
{
	script->Collect(1);
	set<int>* models;
	models = getModels(Params[0].cVar);
	string key(Params[0].cVar);
	if (!removeObjectQueue.contains(key)) {
		removeObjectQueue[key] = models;
	}
	return OR_CONTINUE;
}

// Animation stuff
eOpcodeResult __stdcall playCharAnim(CScript *script)
{
	script->Collect(4);
	CPed* animped = CPools::GetPed(Params[3].nVar);
	plugin::Call<0x405640>(animped->m_pRwClump, Params[1].nVar, Params[0].nVar, Params[2].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall loadTxdDict(CScript* script)
{
	script->Collect(1);
	char fullpath[128];
	snprintf(fullpath, 128, "models\\%s.txd", Params[0].cVar);
	int slot = CTxdStore::FindTxdSlot("script");
	if (slot == -1)
		slot = CTxdStore::AddTxdSlot("script");
	CTxdStore::LoadTxd(slot, fullpath);
	CTxdStore::AddRef(slot);
	return OR_CONTINUE;
}

// Sound

// Sound helper methods
void cleanupSound(string key) {
	if (soundMap.contains(key)) {
		soundMap[key].sound->stop();
		soundMap[key].sound->drop();
		soundMap.erase(key);
	}
}

string getKeyIndex(char* name, int index) {
	string key(name);
	return key + "_" + to_string(index);
}

int findEmptyIndex(char* name) {
	string key;
	int index = 0;
	do {
		key = getKeyIndex(name, ++index);
	} while (soundMap.contains(key));
	return index;
}

// Sound opcodes
eOpcodeResult __stdcall stopAllSounds(CScript* script)
{
	script->Collect(0);
	m_soundEngine->stopAllSounds();
	return OR_CONTINUE;
}

eOpcodeResult __stdcall stopSound(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	cleanupSound(key);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall stopSoundIndex(CScript* script)
{
	script->Collect(2);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	cleanupSound(key);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundPlaying(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	script->UpdateCompareFlag(soundMap.contains(key) && !soundMap[key].sound->isFinished());
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundStopped(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	script->UpdateCompareFlag(!(soundMap.contains(key) && !soundMap[key].sound->isFinished()));
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundPlayingIndex(CScript* script)
{
	script->Collect(2);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	script->UpdateCompareFlag(soundMap.contains(key) && !soundMap[key].sound->isFinished());
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundStoppedIndex(CScript* script)
{
	script->Collect(2);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	script->UpdateCompareFlag(!(soundMap.contains(key) && !soundMap[key].sound->isFinished()));
	return OR_CONTINUE;
}

void __playSound(string key) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", Params[0].cVar);
	cleanupSound(key);
	soundMap[key].sound = m_soundEngine->play2D(fullpath, Params[1].nVar, false, true);
	soundMap[key].spatial = false;
}

eOpcodeResult __stdcall playSound(CScript* script)
{
	script->Collect(2);
	string key(Params[0].cVar);
	__playSound(key);	
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playSoundIndex(CScript* script)
{
	script->Collect(2);
	int index = findEmptyIndex(Params[0].cVar);
	string key = getKeyIndex(Params[0].cVar, index);
	__playSound(key);
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playKeypad(CScript* script)
{
	script->Collect(1);
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%d.wav", Params[0].nVar);
	m_soundEngine->play2D(fullpath);
	return OR_CONTINUE;
}

void __playSoundLocation(string key) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", Params[0].cVar);
	cleanupSound(key);
	vec3df pos;	
	pos.X = Params[1].fVar;
	pos.Y = -1.0f * Params[2].fVar;
	pos.Z = Params[3].fVar;
	soundMap[key].offset = CVector(pos.X, pos.Y, pos.Z);
	soundMap[key].sound = m_soundEngine->play3D(fullpath, pos, Params[4].nVar, false, true);
	soundMap[key].sound->setMinDistance(Params[5].fVar);
	soundMap[key].spatial = true;
}

eOpcodeResult __stdcall playSoundAtLocation(CScript* script)
{
	script->Collect(6);
	string key(Params[0].cVar);	
	__playSoundLocation(key);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playSoundAtLocationIndex(CScript* script)
{
	script->Collect(6);
	int index = findEmptyIndex(Params[0].cVar);
	string key = getKeyIndex(Params[0].cVar, index);
	__playSoundLocation(key);
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

void __attachSoundToVehicle(string key, CVehicle* vehicle) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", Params[0].cVar);
	cleanupSound(key);
	vec3df pos;
	soundMap[key].vehicle = vehicle;
	soundMap[key].offset = CVector(Params[1].fVar, Params[2].fVar, Params[3].fVar);
	Command<Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(vehicle, soundMap[key].offset.x, soundMap[key].offset.y, soundMap[key].offset.z, &pos.X, &pos.Y, &pos.Z);
	pos.Y *= -1.0;
	soundMap[key].sound = m_soundEngine->play3D(fullpath, pos, Params[4].nVar, false, true);
	soundMap[key].sound->setMinDistance(Params[5].fVar);
	soundMap[key].spatial = true;
}

eOpcodeResult __stdcall attachSoundToVehicle(CScript* script)
{
	script->Collect(7);
	int index = 0;
	CVehicle* vehicle = CPools::GetVehicle(Params[6].nVar);
	if (vehicle) {
		string key(Params[0].cVar);
		__attachSoundToVehicle(key, vehicle);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall attachSoundToVehicleIndex(CScript* script)
{
	script->Collect(7);
	int index = 0;
	CVehicle* vehicle = CPools::GetVehicle(Params[6].nVar);
	if (vehicle) {
		index = findEmptyIndex(Params[0].cVar);
		string key = getKeyIndex(Params[0].cVar, index);
		__attachSoundToVehicle(key, vehicle);
	}
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	CVector* pos;
	vec3df playerPos, soundPos, dir;
	string key;
	float distance;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE)hModule);
		Params = CLEO_GetParamsAddress();
		Opcodes::RegisterOpcode(0x3F01, raiseFrontSuspension);
		Opcodes::RegisterOpcode(0x3F02, getEngineStatus);
		Opcodes::RegisterOpcode(0x3F03, turnOnEngine);
		Opcodes::RegisterOpcode(0x3F04, getCurrentGear);
		Opcodes::RegisterOpcode(0x3F05, setHover);
		Opcodes::RegisterOpcode(0x3F06, isDoorOpen);
		Opcodes::RegisterOpcode(0x3F07, playCharAnim);
		Opcodes::RegisterOpcode(0x3F08, addBuilding);
		Opcodes::RegisterOpcode(0x3F09, removeBuilding);
		Opcodes::RegisterOpcode(0x3F10, setCarComponentVisibility);
		Opcodes::RegisterOpcode(0x3F11, setCarComponentIndexVisibility);
		Opcodes::RegisterOpcode(0x3F12, setCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3F13, setCarComponentIndexAlpha);
		Opcodes::RegisterOpcode(0x3F14, moveCarComponent);
		Opcodes::RegisterOpcode(0x3F15, moveCarComponentIndex);
		Opcodes::RegisterOpcode(0x3F16, rotateCarComponent);
		Opcodes::RegisterOpcode(0x3F17, rotateCarComponentIndex);
		Opcodes::RegisterOpcode(0x3F18, setCarComponentGlow);
		Opcodes::RegisterOpcode(0x3F19, setCarComponentGlowIndex);
		Opcodes::RegisterOpcode(0x3F20, getCarOrientation);
		Opcodes::RegisterOpcode(0x3F21, setCarOrientation);
		Opcodes::RegisterOpcode(0x3F22, popWheelie);
		Opcodes::RegisterOpcode(0x3F23, setRemote);
		Opcodes::RegisterOpcode(0x3F24, removeRemote);
		Opcodes::RegisterOpcode(0x3F25, applyForce);
		Opcodes::RegisterOpcode(0x3F26, getWheelStatus);
		Opcodes::RegisterOpcode(0x3F27, setWheelStatus);
		Opcodes::RegisterOpcode(0x3F28, createLight);
		Opcodes::RegisterOpcode(0x3F29, inRemote);
		Opcodes::RegisterOpcode(0x3F30, rotateCar);
		Opcodes::RegisterOpcode(0x3F31, getRotationMatrix);
		Opcodes::RegisterOpcode(0x3F32, setRotationMatrix);
		Opcodes::RegisterOpcode(0x3F33, getVelocity);
		Opcodes::RegisterOpcode(0x3F34, getVelocityVector);
		Opcodes::RegisterOpcode(0x3F35, setVelocityVector);
		Opcodes::RegisterOpcode(0x3F36, getSteeringAngle);
		Opcodes::RegisterOpcode(0x3F37, addObjects);
		Opcodes::RegisterOpcode(0x3F38, removeObjects);
		Opcodes::RegisterOpcode(0x3F40, getCarComponentVisibility);
		Opcodes::RegisterOpcode(0x3F80, stopAllSounds);
		Opcodes::RegisterOpcode(0x3F81, stopSound);
		Opcodes::RegisterOpcode(0x3F82, isSoundPlaying);
		Opcodes::RegisterOpcode(0x3F83, isSoundStopped);
		Opcodes::RegisterOpcode(0x3F84, playSound);
		Opcodes::RegisterOpcode(0x3F85, playSoundAtLocation);
		Opcodes::RegisterOpcode(0x3F86, attachSoundToVehicle);
		Opcodes::RegisterOpcode(0x3F90, playKeypad);
		Opcodes::RegisterOpcode(0x3F91, stopSoundIndex);
		Opcodes::RegisterOpcode(0x3F92, isSoundPlayingIndex);
		Opcodes::RegisterOpcode(0x3F93, isSoundStoppedIndex);
		Opcodes::RegisterOpcode(0x3F94, playSoundIndex);
		Opcodes::RegisterOpcode(0x3F95, playSoundAtLocationIndex);
		Opcodes::RegisterOpcode(0x3F96, attachSoundToVehicleIndex);

		//Opcodes::RegisterOpcode(0x3F37, replaceTex);
		//Opcodes::RegisterOpcode(0x3F38, addCompAnims);
		//Reserving 0x3F18-0x3F1F for get command
		Events::initGameEvent += [] {
			//patch::Nop(0x58E59B, 5, true);
			//patch::Nop(0x58E611, 5, true);
			m_soundEngine = createIrrKlangDevice();
			m_soundEngine->setRolloffFactor(1.5f);
		};

		Events::gameProcessEvent += [&] {
			// Set volume of sound engine to match game
			if (volume != FrontendMenuManager.field_29) {
				volume = FrontendMenuManager.field_29;
				m_soundEngine->setSoundVolume(volume / 127.0f);
			}
			if (Command<Commands::IS_PLAYER_PLAYING>(0))
			{
				pos = TheCamera.GetGameCamPosition();
				playerPos.X = pos->x;
				playerPos.Y = -1.0f * pos->y;
				playerPos.Z = pos->z;
				dir.X = TheCamera.up.x;
				dir.Y = -1.0f * TheCamera.up.y;
				dir.Z = TheCamera.up.z;
				m_soundEngine->setListenerPosition(playerPos, dir, vec3df(0, 0, 0), vec3df(0, 0, 1));
			}
			if (!soundMap.empty()) {
				if (FrontendMenuManager.m_bMenuVisible && !paused) {

					for (auto const& [key, gamesound] : soundMap) {
						gamesound.sound->setIsPaused();
					}
					paused = true;
				}
				else if (!FrontendMenuManager.m_bMenuVisible) {
					auto itr = soundMap.begin();
					while (itr != soundMap.end()) {
						// Delete sound if its finished playing
						if (soundMap[itr->first].sound->isFinished()) {
							itr = soundMap.erase(itr);
							continue;
						}
						// Unpause sound if we're paused
						if (paused) {
							soundMap[itr->first].sound->setIsPaused(false);
						}
						if (soundMap[itr->first].vehicle) {
							// Attach sound to vehicle
							Command<Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(soundMap[itr->first].vehicle, soundMap[itr->first].offset.x, soundMap[itr->first].offset.y, soundMap[itr->first].offset.z, &soundPos.X, &soundPos.Y, &soundPos.Z);
							soundPos.Y *= -1.0;
							soundMap[itr->first].sound->setPosition(soundPos);
						}
						else {
							// Set sound to specified location
							soundPos.X = soundMap[itr->first].offset.x;
							soundPos.Y = soundMap[itr->first].offset.y;
							soundPos.Z = soundMap[itr->first].offset.z;
						}

						// Mute sound if > 150 units away, otherwise play at full volume
						distance = (float)playerPos.getDistanceFrom(soundPos);
						if (distance < 150.0f || !soundMap[itr->first].spatial) {
							soundMap[itr->first].sound->setVolume(1.0f);
						}
						else {
							soundMap[itr->first].sound->setVolume(0.0f);
						}
						++itr;
					}
					paused = false;
				}
			}
			
			// Removes dynamically created objects.  Has to be here because game script tick causes them to flash briefly
			for (auto item : removeObjectQueue) {
				auto models = item.second;
				for (auto object : CPools::ms_pObjectPool) {
					if (models->find(object->m_nModelIndex) != models->end()) {
						CWorld::Remove(object);
					}
				}
				for (auto object : CPools::ms_pDummyPool) {
					if (models->find(object->m_nModelIndex) != models->end()) {
						CWorld::Remove(object);
					}
				}
			}
			/*animEntry* i = &anims[0];

			if (i->timeremain != 0) {
				of << i->timeremain << " " << i->dp.x << " " << i->dp.y << " " << i->dp.z << std::endl;
				CMatrix cmatrix(&i->frame->modelling, false);
				CVector cpos(cmatrix.pos);
				cmatrix.Rotate(i->dr.x, i->dr.y, i->dr.z);
				cmatrix.pos = cpos;
				//cmatrix.SetTranslateOnly(i->dp.x, i->dp.y, i->dp.z);


				cmatrix.UpdateRW();
				i->timeremain -= 1;
			}*/
		};
	}
	return TRUE;
}

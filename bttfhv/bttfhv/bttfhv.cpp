#define _USE_MATH_DEFINES
#include "plugin.h"
#include "CWorld.h"
#include <fstream>
#include <string>
#include <cmath>
#include "VC.CLEO.h"
#pragma comment(lib, "VC.CLEO.lib")
#include "game_vc\CClumpModelInfo.h"
#include "CPools.h"
#include "CFileLoader.h"
#include "CSprite2d.h"
#include "Events.h"

int &ms_atomicPluginOffset = *(int *)0x69A1C8;

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
	data = (void*)(rwObject->flags);
	return rwObject;
}

RwObject* __cdecl TestFlags(RwObject* rwObject, void* data) {
	if (data == (void*)(0))
		rwObject->flags = 0;
	else
		rwObject->privateFlags = 0xF;
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

tScriptVar *Params;
using namespace plugin;

eOpcodeResult __stdcall setSuspensionValues(CScript* script)
{
	script->Collect(1);
	CVehicle* veh = CPools::GetVehicle(Params[0].nVar);
	if (veh) {
		veh->m_pHandlingData->fSuspensionForceLevel = 1.8f;
		veh->m_pHandlingData->fSuspBias = 0.5f;
		veh->m_pHandlingData->fSuspUpperLimit = 0.08f;
		veh->m_pHandlingData->fSuspLowerLimit = -0.02f;
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
			// TODO: Load per vehicle configuration from a file.
			vehicle->FlyingControl(2);
			vehicle->m_pFlyingHandling->fThrust = 0.3f;
			vehicle->m_pFlyingHandling->fThrustFallOff = 0.72f;
			vehicle->m_pFlyingHandling->fYaw = -0.001f;
			vehicle->m_pFlyingHandling->fYawStab = 0.025f;
			vehicle->m_pFlyingHandling->fSideSlip = 0.1f;
			vehicle->m_pFlyingHandling->fRoll = 0.0065f;
			vehicle->m_pFlyingHandling->fRollStab = 8.0f;
			vehicle->m_pFlyingHandling->fPitch = 0.0065f;
			vehicle->m_pFlyingHandling->fPitchStab = 8.0f;
			vehicle->m_pFlyingHandling->fFormLift = 0.7f;
			vehicle->m_pFlyingHandling->fAttackLift = 0.014f;
			vehicle->m_pFlyingHandling->fMoveRes = 0.997f;
			vehicle->m_pFlyingHandling->vecTurnRes.x = 0.82f;
			vehicle->m_pFlyingHandling->vecTurnRes.y = 0.85f;
			vehicle->m_pFlyingHandling->vecTurnRes.z = 0.99f;
			vehicle->m_pFlyingHandling->vecSpeedRes.x = 0.0f;
			vehicle->m_pFlyingHandling->vecSpeedRes.y = 0.0f;
			vehicle->m_pFlyingHandling->vecSpeedRes.z = 10.0f;
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

// Building stuff
eOpcodeResult __stdcall addBuilding(CScript* script)
{
	script->Collect(2);
	char ypath[14];
	strcpy(ypath, "data\\");
	strcat(ypath, Params[0].cVar);
	strcat(ypath, ".dat");
	char* point = ypath;
	//char *ypath = "data\\ ";
	//std::ifstream list(GAME_PATH(ypath.c_str()));
	plugin::config_file config(GAME_PATH(point));
	for (int i = 0; i <= Params[1].nVar; i++) {
		for (auto object : CPools::ms_pBuildingPool) {
			if (object->m_nModelIndex == config[std::to_string(i)].asInt(0)) {

				CWorld::Add(object);
			}
		}
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall removeBuilding(CScript* script)
{
	script->Collect(2);
	char ypath[14];
	strcpy(ypath, "data\\");
	strcat(ypath, Params[0].cVar);
	strcat(ypath, ".dat");
	char* point = ypath;
	//char *ypath = "data\\ ";
	//std::ifstream list(GAME_PATH(ypath.c_str()));
	plugin::config_file config(GAME_PATH(point));
	for (int i = 0; i <= Params[1].nVar; i++) {
		for (auto object : CPools::ms_pBuildingPool) {
			if (object->m_nModelIndex == config[std::to_string(i)].asInt(0)) {

				CWorld::Remove(object);
			}
		}
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Params = CLEO_GetParamsAddress();
		Opcodes::RegisterOpcode(0x3F01, setSuspensionValues);
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

		//Opcodes::RegisterOpcode(0x3F06, createLight);  // Needs to be reimplemented
		//Reserving 0x3F18-0x3F1F for get command
	}
	return TRUE;
}

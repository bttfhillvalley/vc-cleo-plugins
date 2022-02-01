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

// Car stuff
eOpcodeResult __stdcall getCarComponentRotation(CScript* script) {
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	RwFrame* cframedum = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[0].cVar);
	if (cframedum) {
		RwFrame* cframe = cframedum->child;
		CMatrix cmatrix(&cframe->modelling, false);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall moveCarComponent(CScript *script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	RwFrame *cframe = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[0].cVar);
	if (cframe) {
		CMatrix cmmatrix(&cframe->modelling, false);
		//CVector cpos(Params[1].fVar, Params[2].fVar, Params[3].fVar);
		//cmatrix.pos = cpos;
		cmmatrix.SetTranslateOnly(Params[1].fVar, Params[2].fVar, Params[3].fVar);
		cmmatrix.UpdateRW();
	}
	return OR_CONTINUE;
}


eOpcodeResult __stdcall setCarComponentAlpha(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	RwFrame* detail = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[1].cVar);

	if (detail) {
		RpAtomic* atomic;
		RwFrameForAllObjects(detail, GetVehicleAtomicObjectCB, &atomic);
		vehicle->SetComponentAtomicAlpha(atomic, Params[2].nVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentVisibility(CScript *script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[2].nVar);
	RwFrame *detail = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[0].cVar);
	if (detail) {
		CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
		RwFrameForAllObjects(detail, SetVehicleAtomicVisibilityCB, (void *)(Params[1].nVar));
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setCarComponentIndexVisibility(CScript *script)
{
	script->Collect(4);
	CVehicle* vehicle = CPools::GetVehicle(Params[2].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[0].cVar, Params[3].nVar);
	RwFrame *detail = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (detail) {
		RwFrameForAllObjects(detail, SetVehicleAtomicVisibilityCB, (void *)(Params[1].nVar));
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall rotateCarComponent(CScript* script)
{
	script->Collect(5);
	CVehicle* vehicle = CPools::GetVehicle(Params[4].nVar);
	RwFrame* cframe = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, Params[0].cVar);
	if (cframe) {
		CMatrix cmatrix(&cframe->modelling, false);
		CVector cpos(cmatrix.pos);
		cmatrix.SetRotate(radians(Params[1].fVar), radians(Params[2].fVar), radians(Params[3].fVar));
		cmatrix.pos = cpos;
		cmatrix.UpdateRW();
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setHover(CScript *script)
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

eOpcodeResult __stdcall turnOnEngine(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
	CDamageManager* damage = &automobile->m_carDamage;
	damage->SetEngineStatus(250);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getEngineStatus(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[1].nVar);
	CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
	CDamageManager* damage = &automobile->m_carDamage;
	unsigned int status = damage->GetEngineStatus();
	Params[0].nVar = status;
	script->Store(1);
	return OR_CONTINUE;
}

/*eOpcodeResult __stdcall setSuspensionValues(CScript* script)
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

eOpcodeResult __stdcall getCurrentGear(CScript* script)
{
	script->Collect(1);
	CVehicle* veh = CPools::GetVehicle(Params[0].nVar);
	if (veh) {
		Params[0].nVar = (int)veh->m_nCurrentGear;
		script->Store(1);
	}
	return OR_CONTINUE;
}*/


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
		//Opcodes::RegisterOpcode(0x3FF3, setSuspensionValues);
		Opcodes::RegisterOpcode(0x3FF4, setCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3FF5, getEngineStatus);
		Opcodes::RegisterOpcode(0x3FF6, turnOnEngine);
		//Opcodes::RegisterOpcode(0x3FF7, createLight);  // Needs to be reimplemented
		Opcodes::RegisterOpcode(0x3FF8, setCarComponentIndexVisibility);
		Opcodes::RegisterOpcode(0x3FF9, playCharAnim);
		Opcodes::RegisterOpcode(0x3FFA, setHover);
		Opcodes::RegisterOpcode(0x3FFB, setCarComponentVisibility);
		Opcodes::RegisterOpcode(0x3FFC, moveCarComponent);
		Opcodes::RegisterOpcode(0x3FFD, rotateCarComponent);
		Opcodes::RegisterOpcode(0x3FFE, addBuilding);
		Opcodes::RegisterOpcode(0x3FFF, removeBuilding);
	}
	return TRUE;
}

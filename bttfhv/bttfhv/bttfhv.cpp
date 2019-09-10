#include "plugin.h"
#include "CWorld.h"
#include <fstream>
#include <string>
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
	if (data == (void *)(1))
		rwObject->flags = 4;
	else
		rwObject->flags = 0;
	return rwObject;
}

tScriptVar *Params;
using namespace plugin;


/*class bttf {
public:
bttf() {
eOpcodeResult WINAPI OPCODE_0BA2(CScript *script)
{
script->Collect(1);
auto libHandle = MemoryLoadLibrary(Params[0].cVar);
Params[0].pVar = libHandle;
script->Store(1);
script->UpdateCompareFlag(libHandle != nullptr);
return OR_CONTINUE;
}

}
} _bttf;
*/
eOpcodeResult WINAPI OPCODE_REM(CScript *script)
{
	script->Collect(2);
	char ypath[14];
	strcpy(ypath, "data\\");
	strcat(ypath, Params[0].cVar);
	strcat(ypath, ".dat");
	char *point = ypath;
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

eOpcodeResult WINAPI OPCODE_ADD(CScript *script)
{
	script->Collect(2);
	char ypath[14];
	strcpy(ypath, "data\\");
	strcat(ypath, Params[0].cVar);
	strcat(ypath, ".dat");
	char * point = ypath;
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

eOpcodeResult WINAPI OPCODE_ROT(CScript *script)
{
	script->Collect(5);
	CVehicle* veh = CPools::GetVehicle(Params[4].nVar);
	RwFrame *cframedum = CClumpModelInfo::GetFrameFromName(veh->m_pRwClump, Params[0].cVar);
	if (cframedum) {
		RwFrame *cframe = cframedum->child;
		CMatrix cmatrix(&cframe->modelling, false);
		CVector cpos(cmatrix.pos);
		cmatrix.SetRotate(Params[1].fVar, Params[2].fVar, Params[3].fVar);
		cmatrix.pos = cpos;
		cmatrix.UpdateRW();
	}
	return OR_CONTINUE;
}

eOpcodeResult WINAPI OPCODE_MOV(CScript *script)
{
	script->Collect(5);
	CVehicle* veh = CPools::GetVehicle(Params[4].nVar);
	RwFrame *cframedum = CClumpModelInfo::GetFrameFromName(veh->m_pRwClump, Params[0].cVar);
	if (cframedum) {
		RwFrame *cframe = cframedum->child;
		CMatrix cmmatrix(&cframe->modelling, false);
		//CVector cpos(Params[1].fVar, Params[2].fVar, Params[3].fVar);
		//cmatrix.pos = cpos;
		cmmatrix.SetTranslateOnly(Params[1].fVar, Params[2].fVar, Params[3].fVar);
		cmmatrix.UpdateRW();
	}
	return OR_CONTINUE;
}

eOpcodeResult WINAPI OPCODE_SHOWHIDE(CScript *script)
{
	script->Collect(3);
	CVehicle* veh = CPools::GetVehicle(Params[2].nVar);
	RwFrame *detail = CClumpModelInfo::GetFrameFromName(veh->m_pRwClump, Params[0].cVar);
	if (detail) {
		CAutomobile *automobile = reinterpret_cast<CAutomobile *>(veh);
		
			RwFrameForAllObjects(detail, SetVehicleAtomicVisibilityCB, (void *)(Params[1].nVar));
		
	}
	return OR_CONTINUE;
}

eOpcodeResult WINAPI OPCODE_SHOWHIDEN(CScript *script)
{
	script->Collect(4);
	CVehicle* veh = CPools::GetVehicle(Params[2].nVar);
	char tempb[100];
	char tempa[100];
	strcpy(tempb, Params[0].cVar);
	itoa(Params[3].nVar, tempa, 10);
	//strcat(tempb, Params[3].cVar);
	strcat(tempb, tempa);
	RwFrame *detail = CClumpModelInfo::GetFrameFromName(veh->m_pRwClump, tempb);
	if (detail) {
		CAutomobile *automobile = reinterpret_cast<CAutomobile *>(veh);

		RwFrameForAllObjects(detail, SetVehicleAtomicVisibilityCB, (void *)(Params[1].nVar));

	}
	return OR_CONTINUE;
}

eOpcodeResult WINAPI OPCODE_HOVER(CScript *script)
{
	script->Collect(2);
	CVehicle* testcar = CPools::GetVehicle(Params[0].nVar);
	if (testcar) {
		if (Params[1].nVar == 1) {
			testcar->FlyingControl(2);
			testcar->m_pFlyingHandling->fThrust = 0.3f;
			testcar->m_pFlyingHandling->fThrustFallOff = 0.72f;
			testcar->m_pFlyingHandling->fPitch = 0.0045f;
			testcar->m_pFlyingHandling->fRoll = 0.0045f;
			testcar->m_pFlyingHandling->fPitchStab = 8.0000f;
			testcar->m_pFlyingHandling->fRollStab = 8.0000f;
			testcar->m_pFlyingHandling->fMoveRes = 0.9985f;
			testcar->m_pFlyingHandling->fYaw = -0.00100f;
			testcar->m_pFlyingHandling->fYawStab = 0.025f;
			testcar->m_pFlyingHandling->vecTurnRes.x = 0.805f;
			testcar->m_pFlyingHandling->vecTurnRes.y = 0.795f;
			testcar->m_pFlyingHandling->vecTurnRes.z = 0.990f;
			//testcar->Teleport(CVector(0.0, 0.0, 0.0));
		}
		else
		{
			
			testcar->FlyingControl(1);
		}
	}
	
	return OR_CONTINUE;
}

eOpcodeResult WINAPI OPCODE_ANIM(CScript *script)
{
	script->Collect(4);
	CPed* animped = CPools::GetPed(Params[3].nVar);
	plugin::Call<4216384>(animped->m_pRwClump, Params[1].nVar, Params[0].nVar, Params[2].fVar);
	return OR_CONTINUE;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Params = CLEO_GetParamsAddress();
		Opcodes::RegisterOpcode(0x3FFF, OPCODE_REM);
		Opcodes::RegisterOpcode(0x3FFE, OPCODE_ADD);
		Opcodes::RegisterOpcode(0x3FFD, OPCODE_ROT);
		Opcodes::RegisterOpcode(0x3FFC, OPCODE_MOV);
		Opcodes::RegisterOpcode(0x3FFB, OPCODE_SHOWHIDE);
		Opcodes::RegisterOpcode(0x3FFA, OPCODE_HOVER);
		Opcodes::RegisterOpcode(0x3FF9, OPCODE_ANIM);
		Opcodes::RegisterOpcode(0x3FF8, OPCODE_SHOWHIDEN);
	}
	return TRUE;
}

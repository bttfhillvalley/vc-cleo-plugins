#define _USE_MATH_DEFINES
#include "plugin.h"
#include "CWorld.h"
#include <fstream>
#include <string>
#include <cmath>
#include <set>
#include "VC.CLEO.h"
#pragma comment(lib, "VC.CLEO.lib")
#include "game_vc\CClumpModelInfo.h"
#include "CTxdStore.h"
#include "CPools.h"
#include "CFileLoader.h"
#include "CModelInfo.h"
#include "CSimpleModelInfo.h"
#include "cHandlingDataMgr.h"
#include "CPointLights.h"
#include "extensions\ScriptCommands.h"


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

float degrees(float radians) {
	return (float)(radians * 180.0 / M_PI);
}

tScriptVar *Params;
using namespace plugin;

eOpcodeResult __stdcall setSuspensionValues(CScript* script)
{
	script->Collect(1);
	CVehicle* veh = CPools::GetVehicle(Params[0].nVar);
	if (veh) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(veh);
		int i;
		CVector posn;
		CVehicleModelInfo* mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(veh->m_nModelIndex);
		CColModel* colModel = mi->GetColModel();

		// Each suspension line starts at the uppermost wheel position
		// and extends down to the lowermost point on the tyre
		/*for (i = 0; i < 4; i++) {
			mi->GetWheelPosn(i, posn);
			automobile->m_aWheelPosition[i] = posn.z;


			// uppermost wheel position
			posn.z += pHandling->fSuspensionUpperLimit;
			colModel->lines[i].p0 = posn;

			// lowermost wheel position
			posn.z += pHandling->fSuspensionLowerLimit - pHandling->fSuspensionUpperLimit;
			// lowest point on tyre
			posn.z -= mi->m_wheelScale * 0.5f;
			colModel->lines[i].p1 = posn;

			// this is length of the spring at rest
			m_aSuspensionSpringLength[i] = pHandling->fSuspensionUpperLimit - pHandling->fSuspensionLowerLimit;
			m_aSuspensionLineLength[i] = colModel->lines[i].p0.z - colModel->lines[i].p1.z;
		}

		// Compress spring somewhat to get normal height on road
		m_fHeightAboveRoad = m_aSuspensionSpringLength[0] * (1.0f - 1.0f / (4.0f * pHandling->fSuspensionForceLevel))
			- colModel->lines[0].p0.z + mi->m_wheelScale * 0.5f;
		for (i = 0; i < 4; i++)
			m_aWheelPosition[i] = mi->m_wheelScale * 0.5f - m_fHeightAboveRoad;

		// adjust col model to include suspension lines
		if (colModel->boundingBox.min.z > colModel->lines[0].p1.z)
			colModel->boundingBox.min.z = colModel->lines[0].p1.z;
		float radius = Max(colModel->boundingBox.min.Magnitude(), colModel->boundingBox.max.Magnitude());
		if (colModel->boundingSphere.radius < radius)
			colModel->boundingSphere.radius = radius;

		if (GetModelIndex() == MI_RCBANDIT) {
			colModel->boundingSphere.radius = 2.0f;
			for (i = 0; i < colModel->numSpheres; i++)
				colModel->spheres[i].radius = 0.3f;
		}*/


		//veh->SetModelIndex()
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
		x = cos(z);
		y = sin(z);
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

void getModels(std::set<int>* models, char* path) {
	std::ifstream in;
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
					models->emplace(std::stoi(line.substr(0, end)));
				}
			}
			in.close();
		}
	}
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
	script->Collect(2);
	std::set<int> models;
	getModels(&models, Params[0].cVar);
	for (auto object : CPools::ms_pBuildingPool) {
		if (models.find(object->m_nModelIndex) != models.end()) {
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
	script->Collect(2);
	std::set<int> models;
	//std::ofstream of(Params[0].cVar, std::ofstream::out);
	//CSimpleModelInfo* mi;
	getModels(&models, Params[0].cVar);
	//of << "Models Found" << std::endl;
	//for (auto i : models) {
	//	of << i << std::endl;
	//}
	//of << "End" << std::endl;
	//of << "Building Pool" << std::endl;
	for (auto object : CPools::ms_pBuildingPool) {
		if (models.find(object->m_nModelIndex) != models.end()) {
			//of << object->m_nModelIndex << "," << (int)object->m_nType << std::endl;
			CWorld::Remove(object);
		}
	}
	//of << "End" << std::endl;
	//of << "Object Pool" << std::endl;
	/*for (int i = 0; i < 6500; i++) {
		mi = CModelInfo::GetModelInfo(i);
		if (mi) {
			of << i << "," << mi->m_nRefCount << std::endl;
		}
	}*/
	/*for (auto object : CPools::ms_pObjectPool) {
		if (models.find(object->m_nModelIndex) != models.end()) {

			//auto mi = CModelInfo::GetModelInfo(object->m_nModelIndex);
			//object->DeleteRwObject();

			of << object->m_nModelIndex << "," << (int)object->m_nType << std::endl; //  "," << mi->m_nRefCount << std::endl;
			//CWorld::Remove(object);
			//delete(object);
		}
	}*/
	//of << "End" << std::endl;
	/*of << "Dummy Pool" << std::endl;
	for (auto object : CPools::ms_pDummyPool) {
		if (models.find(object->m_nModelIndex) != models.end()) {
			of << object->m_nModelIndex << "," << (int)object->m_nType << std::endl;
			CWorld::Remove(object);
			CWorld:Remove
		}
	}
	of << "End" << std::endl;*/
	//of.close();

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

eOpcodeResult __stdcall addTex(CScript* script) {
	int ref = CTxdStore::AddTxdSlot("ice");
	CTxdStore::LoadTxd(ref, "models/ice.txd");
	CTxdStore::AddRef(ref);
	return OR_CONTINUE;
}


RpMaterial* MaterialCallback(RpMaterial* material, void* texture) {

	//RwTexture* newTexture = reinterpret_cast<RwTexture*>(texture);
	if (material->texture) {
		if (strcmp(material->texture->name, "stainless") == 0) {
			std::ofstream of("DEBUG", std::ofstream::app);
			of << material->texture->name << std::endl;
			RwTexture* replacement = RwTextureRead("ice", NULL);
			material->texture = replacement;
		}
	}
	return material;
}

static RpAtomic* AtomicCallback(RpAtomic* atomic, void* texture) {
	if (atomic->geometry->matList.numMaterials > 0) {
		RpGeometryForAllMaterials(atomic->geometry, MaterialCallback, texture);
	}
	return atomic;
}


//TEXTURESRENDER
eOpcodeResult __stdcall oldReplaceTex(CScript* script)
{
	CTxdStore::PushCurrentTxd();
	script->Collect(1);
	RwTexture* replacement, *original;
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("infernus"));
	replacement = RwTextureRead("ice", NULL);
	if (replacement) {
		original = RwTextureRead("stainless", NULL);
		if (original) {
			memcpy(original, replacement, sizeof(replacement));
		}
		RwTextureDestroy(replacement);
	}
	CTxdStore::PopCurrentTxd();
	return OR_CONTINUE;
}

RwFrame* ListFrameNames(RwFrame* frame, void* data) {
	std::ofstream of("DEBUG", std::ofstream::app);
	of << GetFrameNodeName(frame) << std::endl;
	of.close();
	if (frame->child) {
		RwFrame* child = frame->child;
		do {
			ListFrameNames(child, data);
			child = child->next;
		} while (child);
	}

	return frame;
}

RwObject* __cdecl SetAtomicGlow(RwObject* object, void* data)
{
	RpAtomic* atomic = (RpAtomic*)object;
	RpGeometry* geometry = atomic->geometry;
	std::ofstream of("DEBUG", std::ofstream::app);
	of << GetFrameNodeName((RwFrame*)(atomic->object.object.parent)) << std::endl;
	of.close();
	RpGeometryForAllMaterials(atomic->geometry, MaterialCallback, NULL);
	return object;
}

eOpcodeResult __stdcall replaceTex(CScript* script)
{
	script->Collect(1);
	std::ofstream of("DEBUG", std::ofstream::out);

	//of << "Finding Frame: panels" << std::endl;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
	RpAtomic* atomic;
	RpGeometry* geometry;
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("infernus"));
	for (auto frame : automobile->m_aCarNodes) {
		if (frame) {
			RwFrameForAllObjects(frame, SetAtomicGlow, &atomic);
		}
	}
	AtomicCallback(reinterpret_cast<RpAtomic*>(vehicle->m_pRwObject), NULL);
	of.close();



	/*RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "panels");
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("infernus"));
	RwTexture* replacement = RwTextureRead("ice", NULL);
	//AtomicCallback(reinterpret_cast<RpAtomic*>(vehicle->m_pRwObject), (void*)replacement);
	//RpClumpForAllAtomics(vehicle->m_pRwClump, AtomicCallback, (void*)replacement);

	if (frame) {
		of << "Found Frame" << std::endl;
		RpAtomic* atomic;
		RpGeometry* geometry;
		//RwFrameForAllObjects(frame, GetVehicleAtomicObjectCB, &atomic);
		//RpGeometryForAllMaterials(atomic->geometry, MaterialCallback, (void*)replacement);
		//frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_lf_hi_ok");

		geometry = atomic->geometry;

		of << "Num Materials: " << geometry->matList.numMaterials << std::endl;
		for (int i = 0; i < geometry->matList.numMaterials; i++) {
			of << geometry->matList.materials[i]->texture->name << std::endl;
			if (strcmp(geometry->matList.materials[i]->texture->name, "stainless") == 0) {
				of << "replacing" << std::endl;
				RpMaterial* mat = geometry->matList.materials[i];
				mat->color = geometry->matList.materials[i]->color;
				mat->pad = geometry->matList.materials[i]->pad;
				mat->pipeline = geometry->matList.materials[i]->pipeline;
				mat->refCount = geometry->matList.materials[i]->refCount;
				mat->surfaceProps = geometry->matList.materials[i]->surfaceProps;

				//memcpy(mat, geometry->matList.materials[i], sizeof(geometry->matList.materials[i]));
				mat->texture = replacement;

				//geometry->matList.materials[i] = mat;
			}
		}
	}*/

	return OR_CONTINUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE)hModule);
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
		Opcodes::RegisterOpcode(0x3F36, addTex);
		Opcodes::RegisterOpcode(0x3F37, replaceTex);
		//Reserving 0x3F18-0x3F1F for get command
	}
	return TRUE;
}

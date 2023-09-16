#define _USE_MATH_DEFINES
#include "plugin.h"
#include "irrKlang.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <set>
#include "VC.CLEO.h"
#pragma comment(lib, "VC.CLEO.lib")
#include "CCamera.h"
#include "CClock.h"
#include "CClumpModelInfo.h"
#include "CFileLoader.h"
#include "CFileMgr.h"
#include "CMenuManager.h"
#include "CModelInfo.h"
#include "CParticle.h"
#include "CPointLights.h"
#include "CPools.h"
#include "CSimpleModelInfo.h"
#include "CTimer.h"
#include "CTxdStore.h"
#include "CWorld.h"
#include "CRubbish.h"
#include "cHandlingDataMgr.h"
#include "eEntityStatus.h"
#include "extensions\ScriptCommands.h"

#define GRAVITY (0.008f)
#define Clamp(v, low, high) ((v) < (low) ? (low) : (v) > (high) ? (high) : (v))
#define SQR(x) ((x) * (x))

enum {
	CARWHEEL_FRONT_LEFT,
	CARWHEEL_REAR_LEFT,
	CARWHEEL_FRONT_RIGHT,
	CARWHEEL_REAR_RIGHT
};

enum eSurfaceType
{
	SURFACE_DEFAULT,
	SURFACE_TARMAC,
	SURFACE_GRASS,
	SURFACE_GRAVEL,
	SURFACE_MUD_DRY,
	SURFACE_PAVEMENT,
	SURFACE_CAR,
	SURFACE_GLASS,
	SURFACE_TRANSPARENT_CLOTH,
	SURFACE_GARAGE_DOOR,
	SURFACE_CAR_PANEL,
	SURFACE_THICK_METAL_PLATE,
	SURFACE_SCAFFOLD_POLE,
	SURFACE_LAMP_POST,
	SURFACE_FIRE_HYDRANT,
	SURFACE_GIRDER,
	SURFACE_METAL_CHAIN_FENCE,
	SURFACE_PED,
	SURFACE_SAND,
	SURFACE_WATER,
	SURFACE_WOOD_CRATES,
	SURFACE_WOOD_BENCH,
	SURFACE_WOOD_SOLID,
	SURFACE_RUBBER,
	SURFACE_PLASTIC,
	SURFACE_HEDGE,
	SURFACE_STEEP_CLIFF,
	SURFACE_CONTAINER,
	SURFACE_NEWS_VENDOR,
	SURFACE_WHEELBASE,
	SURFACE_CARDBOARDBOX,
	SURFACE_TRANSPARENT_STONE,
	SURFACE_METAL_GATE,
	SURFACE_SAND_BEACH,
	SURFACE_CONCRETE_BEACH,
};

using namespace irrklang;
using namespace plugin;
using namespace std;

ISoundEngine* m_soundEngine;
bool loadedSound = false;
char volume = 0;
int visibility;  // Hack for now, not sure how to pass this in as a (void*) without messing up the value
boolean paused = false;
unsigned char work_buff[55000];

struct GameSound {
	ISound* sound;
	CVehicle* vehicle;
	CVector offset{ 0.0,0.0,0.0 };
	bool spatial;
};

struct CarAttachments {
	CVehicle* vehicle;
	CVehicle* attached;
	CMatrix offset;
};

map<string, GameSound> soundMap;
map<int, CarAttachments> carAttachments;
map<string, set<int>> ideMap;
map<string, set<int>*> removeObjectQueue;
map<string, tHandlingData*> handlingData;
map<string, tFlyingHandlingData*> flyingHandlingData;
map<string, tBoatHandlingData*> boatHandlingData;
map<string, tBikeHandlingData*> bikeHandlingData;

int &ms_atomicPluginOffset = *(int *)0x69A1C8;
//ofstream of("DEBUG", std::ofstream::app);

int __cdecl GetAtomicId(RpAtomic *atomic) {
	return *(&atomic->object.object.type + ms_atomicPluginOffset);
}

RpMaterial* __cdecl SetAmbientCB(RpMaterial* material, void* data)
{
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	if (data == (void*)(0))
		properties->ambient = 0.5f;
	else
		properties->ambient = 5.0f;
	return material;
}

RpMaterial* SetRedCB(RpMaterial* material, void* data)
{
	unsigned int value = reinterpret_cast<unsigned int>(data);
	RwRGBA* col = (RwRGBA*)RpMaterialGetColor(material);	// get rid of const
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	col->red = value;
	return material;
}

RpMaterial* SetGreenCB(RpMaterial* material, void* data)
{
	unsigned int value = reinterpret_cast<unsigned int>(data);
	RwRGBA* col = (RwRGBA*)RpMaterialGetColor(material);	// get rid of const
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	col->green = value;
	return material;
}

RpMaterial* SetBlueCB(RpMaterial* material, void* data)
{
	unsigned int value = reinterpret_cast<unsigned int>(data);
	RwRGBA* col = (RwRGBA*)RpMaterialGetColor(material);	// get rid of const
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	col->blue = value;
	return material;
}

RwObject *__cdecl SetAtomicVisibilityCB(RwObject *rwObject, void *data) {
	if (data == (void *)(0))
		rwObject->flags = 0;
	else
		rwObject->flags = 4;
	return rwObject;
}

RwObject* __cdecl GetAtomicVisibilityCB(RwObject* rwObject, void* data) {
	visibility = (int)rwObject->flags;
	return rwObject;
}

RwObject *__cdecl GetAtomicObjectCB(RwObject* object, void* data)
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

inline float DotProduct(const CVector& v1, const CVector& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

tScriptVar *Params;
using namespace plugin;

void HoverControl(CVehicle* vehicle, bool landing, bool damaged)
{
	if (vehicle->m_pFlyingHandling == nullptr)
		return;
	float fThrust = 0.0f;
	float fPitch = 0.0f;
	float fRoll = 0.0f;
	float fYaw = 0.0f;
	float fUp = 1.0f;
	float fLandingSpeed = 0.1f;
	if (landing) {
		fUp -= fLandingSpeed;
	}
	tFlyingHandlingData* flyingHandling = vehicle->m_pFlyingHandling;
	float rm = pow(flyingHandling->fMoveRes, CTimer::ms_fTimeStep);
	float fUpSpeed = DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.at);
	float fAttitude = asin(vehicle->m_placement.up.z);
	float fAttitudeUp = fAttitude + radians(90.0f);
	float fHeading = atan2(vehicle->m_placement.up.y, vehicle->m_placement.up.x);
	if (FindPlayerVehicle() == vehicle || vehicle->m_nState == STATUS_PLAYER_REMOTE) {
		fThrust = (CPad::GetPad(0)->GetAccelerate() - CPad::GetPad(0)->GetBrake()) / 255.0f;
		fPitch = CPad::GetPad(0)->GetSteeringUpDown() / 128.0f;
		if (CPad::GetPad(0)->PCTempJoyState.RightStickY == CPad::GetPad(0)->GetCarGunUpDown() && abs(CPad::GetPad(0)->PCTempJoyState.RightStickY) > 1.0f) {
			fThrust = CPad::GetPad(0)->GetCarGunUpDown() / 128.0f;
		}
		else if (abs(CPad::GetPad(0)->LookAroundUpDown()) > 1.0f) {
			fPitch = CPad::GetPad(0)->LookAroundUpDown() / 128.0f;
		}
		fRoll = -CPad::GetPad(0)->GetSteeringLeftRight() / 128.0f;
		fYaw = CPad::GetPad(0)->GetCarGunLeftRight() / 128.0f;
		if (landing) {
			if (vehicle->m_placement.at.z > 0.0f) {
				fThrust = Clamp(fThrust, -1.0f * abs(cos(fAttitude)), 0.0f);
			}
			else {
				fThrust = Clamp(fThrust, 0.0f, 1.0f * abs(cos(fAttitude)));
			}
		}
	}
	else if (vehicle->m_nState == STATUS_PHYSICS) {
		if (landing) {
			fThrust = 0.0f;
		}
		else {
			fThrust = -fLandingSpeed / flyingHandling->fThrust;
		}
		fYaw = 0.0f;
		fPitch = Clamp(0.5f * DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.up), -0.1f, 0.1f);
		fRoll = Clamp(0.5f * -vehicle->m_placement.right.z, -0.1f, 0.1f);
		rm *= 0.97f;
	}
	vehicle->m_vecMoveSpeed *= rm;
	if (!damaged) {
		fThrust = flyingHandling->fThrust * fThrust;
		if (vehicle->GetPosition().z > 1000.0f)
			fThrust *= 10.0f / (vehicle->GetPosition().z - 70.0f);

		vehicle->ApplyMoveForce(GRAVITY * vehicle->m_placement.at * fThrust * vehicle->m_fMass * CTimer::ms_fTimeStep);
	}
	else {
		fPitch = Clamp(fPitch * Clamp(5.0f - abs(vehicle->m_vecTurnSpeed.x), 0.0f, 5.0f) / 5.0f, -0.5f, 0.5f);
		fRoll = Clamp(fRoll * Clamp(5.0f - abs(vehicle->m_vecTurnSpeed.y), 0.0f, 5.0f) / 5.0f, -0.5f, 0.5f);
		fYaw = Clamp(fYaw * Clamp(5.0f - abs(vehicle->m_vecTurnSpeed.z), 0.0f, 5.0f) / 5.0f, -0.5f, 0.5f);
	}

	if (damaged) {
		fUp = 0.4f;
	}

	if (vehicle->m_nState == STATUS_PLAYER || vehicle->m_nState == STATUS_PLAYER_REMOTE || vehicle->m_nState == STATUS_PHYSICS) {
		// Hover
		CVector upVector(cos(fAttitudeUp) * cos(fHeading), cos(fAttitudeUp) * sin(fHeading), sin(fAttitudeUp));
		upVector.Normalise();

		if (!damaged) {
			float fLiftSpeed = DotProduct(vehicle->m_vecMoveSpeed, upVector);
			fUp -= flyingHandling->fThrustFallOff * fLiftSpeed;
		}
		fUp *= cos(fAttitude);

		vehicle->ApplyMoveForce(GRAVITY * upVector * fUp * vehicle->m_fMass * CTimer::ms_fTimeStep);
	}

	if (vehicle->m_placement.at.z > 0.0f) {
		float upRight = Clamp(vehicle->m_placement.right.z, -flyingHandling->fFormLift, flyingHandling->fFormLift);
		float upImpulseRight = -upRight * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseRight * vehicle->m_placement.at, vehicle->m_placement.right);

		float upFwd = Clamp(vehicle->m_placement.up.z, -flyingHandling->fFormLift, flyingHandling->fFormLift);
		float upImpulseFwd = -upFwd * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseFwd * vehicle->m_placement.at, vehicle->m_placement.up);
	}
	else {
		float upRight = vehicle->m_placement.right.z < 0.0f ? -flyingHandling->fFormLift : flyingHandling->fFormLift;
		float upImpulseRight = -upRight * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseRight * vehicle->m_placement.at, vehicle->m_placement.right);

		float upFwd = vehicle->m_placement.up.z < 0.0f ? -flyingHandling->fFormLift : flyingHandling->fFormLift;
		float upImpulseFwd = -upFwd * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseFwd * vehicle->m_placement.at, vehicle->m_placement.up);
	}

	vehicle->ApplyTurnForce(fPitch * vehicle->m_placement.at * flyingHandling->fPitch * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, vehicle->m_placement.up);
	vehicle->ApplyTurnForce(fRoll * vehicle->m_placement.at * flyingHandling->fRoll * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, vehicle->m_placement.right);

	float fSideSpeed = -DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.right);
	float fSideSlipAccel = flyingHandling->fSideSlip * fSideSpeed * abs(fSideSpeed);
	vehicle->ApplyMoveForce(vehicle->m_fMass * vehicle->m_placement.right * fSideSlipAccel * CTimer::ms_fTimeStep);

	float fSideSpeedB = -DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.at);
	float fSideSlipAccelB = flyingHandling->fSideSlip * fSideSpeedB * abs(fSideSpeedB);
	vehicle->ApplyMoveForce(vehicle->m_fMass * vehicle->m_placement.at * fSideSlipAccelB * CTimer::ms_fTimeStep);

	if (landing) {
		float fSideSpeedC = -DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.up);
		float fSideSlipAccelC = flyingHandling->fSideSlip * fSideSpeedC * abs(fSideSpeedC);
		fSideSlipAccelC /= 20.0f;
		vehicle->ApplyMoveForce(vehicle->m_fMass* vehicle->m_placement.up* fSideSlipAccelC* CTimer::ms_fTimeStep);
	}

	float fYawAccel = flyingHandling->fYawStab * fSideSpeed * abs(fSideSpeed) + flyingHandling->fYaw * fYaw;
	vehicle->ApplyTurnForce(fYawAccel * vehicle->m_placement.right * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, -vehicle->m_placement.up);
	vehicle->ApplyTurnForce(fYaw * vehicle->m_placement.up * flyingHandling->fYaw * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, vehicle->m_placement.right);
	if (!damaged) {
		float rX = pow(flyingHandling->vecTurnRes.x, CTimer::ms_fTimeStep);
		float rY = pow(flyingHandling->vecTurnRes.y, CTimer::ms_fTimeStep);
		float rZ = pow(flyingHandling->vecTurnRes.z, CTimer::ms_fTimeStep);
		CVector vecTurnSpeed = Multiply3x3(vehicle->m_vecTurnSpeed, vehicle->m_placement);

		float fResistanceMultiplier = powf(1.0f / (flyingHandling->vecSpeedRes.z * SQR(vecTurnSpeed.z) + 1.0f) * rZ, CTimer::ms_fTimeStep);
		float fResistance = vecTurnSpeed.z * fResistanceMultiplier - vecTurnSpeed.z;
		vecTurnSpeed.x *= rX;
		vecTurnSpeed.y *= rY;
		vecTurnSpeed.z *= fResistanceMultiplier;
		vehicle->m_vecTurnSpeed = Multiply3x3(vehicle->m_placement, vecTurnSpeed);
		vehicle->ApplyTurnForce(-vehicle->m_placement.right * fResistance * vehicle->m_fTurnMass, vehicle->m_placement.up + Multiply3x3(vehicle->m_placement, vehicle->m_vecCentreOfMass));

		if (FindPlayerVehicle() == vehicle) {
			CPlayerInfo* player = FindPlayerPed()->GetPlayerInfoForThisPlayerPed();
			player->m_nUpsideDownCounter = 0;
		}
	}
}

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

eOpcodeResult __stdcall updateHandling(CScript* script)
{
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
		CVehicleModelInfo* modelInfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));

		string name(modelInfo->m_szName);
		if (!name.empty()) {
			if (handlingData.contains(name)) {
				vehicle->m_pHandlingData = handlingData[name];
				automobile->SetupSuspensionLines();
			}
			if (flyingHandlingData.contains(name)) {
				vehicle->m_pFlyingHandling = flyingHandlingData[name];
			}
		}
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
			automobile->fWheelRot[n] = radians(Params[n+1].fVar);
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
			automobile->fWheelSpeed[n] =Params[n + 1].fVar;
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

// Car stuff
// Helper methods
void setVisibility(CEntity* model, char* component, int visible) {
	visibility = 0;
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
	if (frame) {
		RwFrameForAllObjects(frame, GetAtomicVisibilityCB, NULL);
		if (visible != visibility) {
			RwFrameForAllObjects(frame, SetAtomicVisibilityCB, (void*)visible);
		}
	}
}

void getVisibility(CEntity* model, char* component) {
	visibility = 0;
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
	if (frame) {
		RwFrameForAllObjects(frame, GetAtomicVisibilityCB, NULL);
	}
}

void moveComponent(CEntity* model, char* component, float x, float y, float z) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
	if (frame) {
		CMatrix cmmatrix(&frame->modelling, false);
		cmmatrix.SetTranslateOnly(x, y, z);
		cmmatrix.UpdateRW();
	}
}

void rotateComponent(CEntity* model, char* component, float rx, float ry, float rz) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
	if (frame) {
		CMatrix cmatrix(&frame->modelling, false);
		CVector cpos(cmatrix.pos);
		cmatrix.SetRotate(radians(rx), radians(ry), radians(rz));
		cmatrix.pos = cpos;
		cmatrix.UpdateRW();
	}
}

void setColor(CVehicle* vehicle, char* component, int red, int green, int blue) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		RpGeometryForAllMaterials(geometry, SetRedCB, (void*)red);
		RpGeometryForAllMaterials(geometry, SetGreenCB, (void*)green);
		RpGeometryForAllMaterials(geometry, SetBlueCB, (void*)blue);
	}
}

void setAlpha(CVehicle* vehicle, char* component, int alpha) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		vehicle->SetComponentAtomicAlpha(atomic, alpha);
		RwFrameForAllObjects(frame, SetAtomicVisibilityCB, (void*)alpha);
	}
}

RwUInt8 getAlpha(CVehicle* vehicle, char* component) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	RwUInt8 alpha = 0;
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		alpha = atomic->geometry->matList.materials[0]->color.alpha;
	}
	return alpha;
}

void fadeAlpha(CVehicle* vehicle, char* component, int target, int fade) {
	int alpha = getAlpha(vehicle, component);
	target = max(0, target);
	target = min(target, 255);
	if (alpha > target) {
		alpha -= fade;
		alpha = max(alpha, target);
	}
	else if (alpha < target) {
		alpha += fade;
		alpha = min(alpha, target);
	}
	setAlpha(vehicle, component, alpha);
}

void setGlow(CVehicle* vehicle, char* component, int glow) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		RpGeometryForAllMaterials(geometry, SetAmbientCB, (void*)glow);
	}
}

int getCurrentDigit(CVehicle* vehicle, char* component) {
	char digitComponent[128];
	for (int digit = 0; digit < 20; digit++) {
		sprintf(digitComponent, "%s%d", component, digit);
		getVisibility(vehicle, digitComponent);
		if (visibility > 0) {
			return digit;
		}
	}
	return -1;
}

void digitOff(CVehicle* vehicle, char* component) {
	char digitComponent[128];
	int digit = getCurrentDigit(vehicle, component);
	if (digit != -1) {
		sprintf(digitComponent, "%s%d", component, digit);
		setVisibility(vehicle, digitComponent, 0);
	}
}

void digitOn(CVehicle* vehicle, char* component, int digit) {
	if (digit == -1) {
		digitOff(vehicle, component);
		return;
	}
	char digitComponent[128];
	sprintf(digitComponent, "%s%d", component, digit);
	getVisibility(vehicle, digitComponent);
	if (visibility == 0) {
		digitOff(vehicle, component);
		setVisibility(vehicle, digitComponent, 1);
	}
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
eOpcodeResult __stdcall setPlayerVisibility(CScript* script)
{
	script->Collect(1);
	CPed* player;
	Command<Commands::GET_PLAYER_CHAR>(0, &player);
	player->SetRwObjectAlpha(Params[0].nVar);
	return OR_CONTINUE;
}

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
	getVisibility(vehicle, Params[1].cVar);
	script->UpdateCompareFlag(visibility != 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentNotVisible(CScript* script)
{
	script->Collect(2);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	getVisibility(vehicle, Params[1].cVar);
	script->UpdateCompareFlag(visibility == 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentIndexVisible(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	getVisibility(vehicle, component);
	script->UpdateCompareFlag(visibility != 0);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isCarComponentIndexNotVisible(CScript* script)
{
	script->Collect(3);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	char component[256];
	sprintf(component, "%s%d", Params[1].cVar, Params[2].nVar);
	getVisibility(vehicle, component);
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

eOpcodeResult __stdcall moveCarComponent(CScript *script)
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
		automobile->OpenDoor(CAR_BONNET, BONNET, Params[1].fVar);
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
	script->Collect(1);
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	CAutomobile* automobile;
	if (vehicle) {
		automobile = reinterpret_cast<CAutomobile*>(vehicle);
		for (int i = 0; i < 4; i++) {
			if (automobile->fWheelSuspDist[i] < 1.0f) {
				static float speedSq;
				speedSq = automobile->m_vecMoveSpeed.Magnitude();
				if (speedSq > SQR(0.1f) &&
					automobile->stWheels[i].surfaceB != SURFACE_GRASS &&
					automobile->stWheels[i].surfaceB != SURFACE_MUD_DRY &&
					automobile->stWheels[i].surfaceB != SURFACE_SAND &&
					automobile->stWheels[i].surfaceB != SURFACE_SAND_BEACH &&
					automobile->stWheels[i].surfaceB != SURFACE_WATER) {
					CVector normalSpeed = automobile->stWheels[i].vecWheelAngle * DotProduct(automobile->stWheels[i].vecWheelAngle, automobile->m_vecMoveSpeed);
					CVector frictionSpeed = automobile->m_vecMoveSpeed - normalSpeed;
					if (i == CARWHEEL_FRONT_LEFT || i == CARWHEEL_REAR_LEFT)
						frictionSpeed -= 0.05f * automobile->m_placement.right;
					else
						frictionSpeed += 0.05f * automobile->m_placement.right;
					CVector sparkDir = 0.25f * frictionSpeed;
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

eOpcodeResult __stdcall addTex(CScript* script) {
	int ref = CTxdStore::AddTxdSlot("ice");
	CTxdStore::LoadTxd(ref, "models/ice.txd");
	CTxdStore::AddRef(ref);
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
	CVehicle* attached = CPools::GetVehicle(index);
	CVehicle* vehicle = CPools::GetVehicle(Params[1].nVar);
	if (vehicle && attached) {
		carAttachments[index].vehicle = vehicle;
		carAttachments[index].attached = attached;
		carAttachments[index].offset = Invert(vehicle->m_placement) * attached->m_placement;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall detachVehicle(CScript* script) {
	script->Collect(1);
	int index = Params[0].nVar;
	if (carAttachments.contains(index)) {
		carAttachments.erase(index);
	}
	return OR_CONTINUE;
}

RpMaterial* MaterialCallback(RpMaterial* material, void* texture) {

	//RwTexture* newTexture = reinterpret_cast<RwTexture*>(texture);
	if (material->texture) {
		if (strcmp(material->texture->name, "stainless") == 0) {
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

RwFrame* ListFrameNames(RwFrame* frame, void* data) {
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
	RpGeometryForAllMaterials(atomic->geometry, MaterialCallback, NULL);
	return object;
}


//TEXTURESRENDER

eOpcodeResult __stdcall oldReplaceTex(CScript* script)
{
	CTxdStore::PushCurrentTxd();
	script->Collect(5);
	RwTexture *replacement, *original;
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot(Params[1].cVar));
	char texture[256];
	sprintf(texture, "%s%d", Params[3].cVar, Params[4].nVar);
	replacement = RwTextureRead(texture, NULL);
	if (replacement) {
		original = RwTextureRead(Params[2].cVar, NULL);
		if (original) {
			memcpy(original, replacement, sizeof(replacement));
		}
		RwTextureDestroy(replacement);
	}
	CTxdStore::PopCurrentTxd();
	return OR_CONTINUE;
}

eOpcodeResult __stdcall replaceTex(CScript* script)
{
	CTxdStore::PushCurrentTxd();
	script->Collect(1);
	std::ofstream of("DEBUG", std::ofstream::out);

	of << "Finding Frame: panels" << std::endl;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	/*CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
	RpAtomic* atomic;
	RpGeometry* geometry;
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("infernus"));
	for (auto frame : automobile->m_aCarNodes) {
		if (frame) {
			RwFrameForAllObjects(frame, SetAtomicGlow, &atomic);
		}
	}
	AtomicCallback(reinterpret_cast<RpAtomic*>(vehicle->m_pRwObject), NULL);
	of.close();*/
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "panels");
		//AtomicCallback(reinterpret_cast<RpAtomic*>(vehicle->m_pRwObject), (void*)replacement);
		//RpClumpForAllAtomics(vehicle->m_pRwClump, AtomicCallback, (void*)replacement);
		if (frame) {
			CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("hotring"));
			RwTexture* replacement = RwTextureRead("ice", NULL);
			of << "Found Frame" << std::endl;
			RpAtomic* atomic;
			//RpGeometry* geometry;
			RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
			RpGeometryForAllMaterials(atomic->geometry, MaterialCallback, (void*)replacement);
			//frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_lf_hi_ok");
			/*geometry = atomic->geometry;
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
			}*/
		}
	}
	CTxdStore::PopCurrentTxd();
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

eOpcodeResult __stdcall setBuildingComponentVisibility(CScript* script)
{
	script->Collect(3);
	for (auto object : CPools::ms_pBuildingPool) {
		if (object->m_nModelIndex == Params[0].nVar) {
			setVisibility(object, Params[1].cVar, Params[2].nVar);
			break;
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
	if (!filesystem::exists(fullpath)) {
		return;
	}
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
	if (filesystem::exists(fullpath)) {
		m_soundEngine->play2D(fullpath);
	}
	return OR_CONTINUE;
}

void __playSoundLocation(string key) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", Params[0].cVar);
	if (!filesystem::exists(fullpath)) {
		return;
	}
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
	snprintf(fullpath, 128, ".\\sound\\%s", Params[1].cVar);
	if (!filesystem::exists(fullpath)) {
		return;
	}
	cleanupSound(key);
	vec3df pos;
	soundMap[key].vehicle = vehicle;
	soundMap[key].offset = CVector(Params[2].fVar, Params[3].fVar, Params[4].fVar);
	Command<Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(vehicle, soundMap[key].offset.x, soundMap[key].offset.y, soundMap[key].offset.z, &pos.X, &pos.Y, &pos.Z);
	pos.Y *= -1.0;

	soundMap[key].sound = m_soundEngine->play3D(fullpath, pos, Params[5].nVar, false, true);
	soundMap[key].sound->setMinDistance(Params[6].fVar);
	soundMap[key].spatial = true;
}

eOpcodeResult __stdcall attachSoundToVehicle(CScript* script)
{
	script->Collect(7);
	int index = 0;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		string key= getKeyIndex(Params[1].cVar, Params[0].nVar);
		__attachSoundToVehicle(key, vehicle);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setFrequency(CScript* script)
{
	script->Collect(3);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	if (soundMap.contains(key)) {
		soundMap[key].sound->setPlaybackSpeed(Params[2].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setVolume(CScript* script)
{
	script->Collect(3);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	if (soundMap.contains(key)) {
		soundMap[key].sound->setVolume(Params[2].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall attachSoundToVehicleIndex(CScript* script)
{
	script->Collect(7);
	int index = 0;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		index = findEmptyIndex(Params[1].cVar);
		string key = getKeyIndex(Params[1].cVar, index);
		__attachSoundToVehicle(key, vehicle);
	}
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setDoppler(CScript* script)
{
	script->Collect(2);
	m_soundEngine->setDopplerEffectParameters(Params[0].fVar, Params[1].fVar);
	return OR_CONTINUE;
}

void ConvertBikeDataToGameUnits(tBikeHandlingData* handling)
{
	handling->m_fMaxLean = sin(radians(handling->m_fMaxLean));
	handling->m_fFullAnimLean = radians(handling->m_fFullAnimLean);
	handling->m_fWheelieAng = sin(radians(handling->m_fWheelieAng));
	handling->m_fStoppieAng = sin(radians(handling->m_fStoppieAng));
}

bool doesFileExist(const char* filepath) {
	fstream infile(".\\DATA\\HANDLING_ADDITIONAL.CFG");
	return infile.good();
}

void LoadAdditionalHandlingData(void)
{
	char* start, * end;
	char line[201];	// weird value
	char delim[4];	// not sure
	char* word;
	string key;
	int field;
	int keepGoing;
	tHandlingData* handling;
	tFlyingHandlingData* flyingHandling;
	tBoatHandlingData* boatHandling;
	tBikeHandlingData* bikeHandling;

	if (!doesFileExist(".\\DATA\\HANDLING_ADDITIONAL.CFG")) {
		return;
	}

	CFileMgr::SetDir("DATA");
	CFileMgr::LoadFile("HANDLING_ADDITIONAL.CFG", work_buff, sizeof(work_buff), "r");
	CFileMgr::SetDir("");

	start = (char*)work_buff;
	end = start + 1;
	keepGoing = 1;

	while (keepGoing) {
		// find end of line
		while (*end != '\n') end++;

		// get line
		strncpy(line, start, end - start);
		line[end - start] = '\0';
		start = end + 1;
		end = start + 1;

		// yeah, this is kinda crappy
		if (strcmp(line, ";the end") == 0)
			keepGoing = 0;
		else if (line[0] != ';') {
			if (line[0] == '!') {
				// Bike data
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0: break;
					case  1:
						key = string(word);
						bikeHandling = new tBikeHandlingData();
						bikeHandlingData[key] = bikeHandling;
						break;
					case  2: bikeHandling->m_fLeanFwdCOM = (float)atof(word); break;
					case  3: bikeHandling->m_fLeanFwdForce = (float)atof(word); break;
					case  4: bikeHandling->m_fLeanBakCOM = (float)atof(word); break;
					case  5: bikeHandling->m_fLeanBakForce = (float)atof(word); break;
					case  6: bikeHandling->m_fMaxLean = (float)atof(word); break;
					case  7: bikeHandling->m_fFullAnimLean = (float)atof(word); break;
					case  8: bikeHandling->m_fDesLean = (float)atof(word); break;
					case  9: bikeHandling->m_fSpeedSteer = (float)atof(word); break;
					case 10: bikeHandling->m_fSlipSteer = (float)atof(word); break;
					case 11: bikeHandling->m_fNoPlayerCOMz = (float)atof(word); break;
					case 12: bikeHandling->m_fWheelieAng = (float)atof(word); break;
					case 13: bikeHandling->m_fStoppieAng = (float)atof(word); break;
					case 14: bikeHandling->m_fWheelieSteer = (float)atof(word); break;
					case 15: bikeHandling->m_fWheelieStabMult = (float)atof(word); break;
					case 16: bikeHandling->m_fStoppieStabMult = (float)atof(word); break;
					}
					field++;
				}
				ConvertBikeDataToGameUnits(bikeHandling);
			}
			else if (line[0] == '$') {
				// Flying data
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0: break;
					case  1:
						key = string(word);
						flyingHandling = new tFlyingHandlingData();
						flyingHandlingData[key] = flyingHandling;
						break;
					case  2: flyingHandling->fThrust = (float)atof(word); break;
					case  3: flyingHandling->fThrustFallOff = (float)atof(word); break;
					case  4: flyingHandling->fYaw = (float)atof(word); break;
					case  5: flyingHandling->fYawStab = (float)atof(word); break;
					case  6: flyingHandling->fSideSlip = (float)atof(word); break;
					case  7: flyingHandling->fRoll = (float)atof(word); break;
					case  8: flyingHandling->fRollStab = (float)atof(word); break;
					case  9: flyingHandling->fPitch = (float)atof(word); break;
					case 10: flyingHandling->fPitchStab = (float)atof(word); break;
					case 11: flyingHandling->fFormLift = (float)atof(word); break;
					case 12: flyingHandling->fAttackLift = (float)atof(word); break;
					case 13: flyingHandling->fMoveRes = (float)atof(word); break;
					case 14: flyingHandling->vecTurnRes.x = (float)atof(word); break;
					case 15: flyingHandling->vecTurnRes.y = (float)atof(word); break;
					case 16: flyingHandling->vecTurnRes.z = (float)atof(word); break;
					case 17: flyingHandling->vecSpeedRes.x = (float)atof(word); break;
					case 18: flyingHandling->vecSpeedRes.y = (float)atof(word); break;
					case 19: flyingHandling->vecSpeedRes.z = (float)atof(word); break;
					}
					field++;
				}
			}
			else if (line[0] == '%') {
				// Boat data
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0: break;
					case  1:
						key = string(word);
						boatHandling = new tBoatHandlingData();
						boatHandlingData[key] = boatHandling;
						break;
					case  2: boatHandling->m_fThrustY = (float)atof(word); break;
					case  3: boatHandling->m_fThrustZ = (float)atof(word); break;
					case  4: boatHandling->m_fThrustAppZ = (float)atof(word); break;
					case  5: boatHandling->m_fAqPlaneForce = (float)atof(word); break;
					case  6: boatHandling->m_fAqPlaneLimit = (float)atof(word); break;
					case  7: boatHandling->m_fAqPlaneOffset = (float)atof(word); break;
					case  8: boatHandling->m_fWaveAudioMult = (float)atof(word); break;
					case  9: boatHandling->m_vMoveRes.x = (float)atof(word); break;
					case 10: boatHandling->m_vMoveRes.y = (float)atof(word); break;
					case 11: boatHandling->m_vMoveRes.z = (float)atof(word); break;
					case 12: boatHandling->m_vTurnRes.x = (float)atof(word); break;
					case 13: boatHandling->m_vTurnRes.y = (float)atof(word); break;
					case 14: boatHandling->m_vTurnRes.z = (float)atof(word); break;
					case 15: boatHandling->m_fLookLRBehindCamHeight = (float)atof(word); break;
					}
					field++;
				}
			}
			else {
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0:
						key = string(word);
						handling = new tHandlingData();
						handlingData[key] = handling;
						break;
					case  1: handling->fMass = (float)atof(word); break;
					case  2: handling->m_vDimensions.x = (float)atof(word); break;
					case  3: handling->m_vDimensions.y = (float)atof(word); break;
					case  4: handling->m_vDimensions.z = (float)atof(word); break;
					case  5: handling->m_vecCentreOfMass.x = (float)atof(word); break;
					case  6: handling->m_vecCentreOfMass.y = (float)atof(word); break;
					case  7: handling->m_vecCentreOfMass.z = (float)atof(word); break;
					case  8: handling->nPercentSubmerged = atoi(word); break;
					case  9: handling->fTractionMultiplier = (float)atof(word); break;
					case 10: handling->fTractionLoss = (float)atof(word); break;
					case 11: handling->fTractionBias = (float)atof(word); break;
					case 12: handling->m_transmissionData.m_nNumberOfGears = atoi(word); break;
					case 13: handling->m_transmissionData.m_fMaxGearVelocity = (float)atof(word); break;
					case 14: handling->m_transmissionData.m_fEngineAcceleration = (float)atof(word) * 0.4f; break;
					case 15: handling->m_transmissionData.m_nDriveType = word[0]; break;
					case 16: handling->m_transmissionData.m_nEngineType = word[0]; break;
					case 17: handling->fBrakeDeceleration = (float)atof(word); break;
					case 18: handling->fBrakeBias = (float)atof(word); break;
					case 19: handling->bABS = !!atoi(word); break;
					case 20: handling->fSteeringLock = (float)atof(word); break;
					case 21: handling->fSuspensionForceLevel = (float)atof(word); break;
					case 22: handling->fSuspensionDampingLevel = (float)atof(word); break;
					case 23: handling->fSeatOffsetDistance = (float)atof(word); break;
					case 24: handling->fCollisionDamageMultiplier = (float)atof(word); break;
					case 25: handling->nMonetaryValue = atoi(word); break;
					case 26: handling->fSuspUpperLimit= (float)atof(word); break;
					case 27: handling->fSuspLowerLimit = (float)atof(word); break;
					case 28: handling->fSuspBias = (float)atof(word); break;
					case 29: handling->fSuspAntiDiveMultiplier = (float)atof(word); break;
					case 30:
						sscanf(word, "%x", &handling->uFlags);
						handling->m_transmissionData.m_nHandlingFlags = handling->uFlags;
						break;
					case 31: handling->bFrontLights = eVehicleLightsSize(atoi(word)); break;
					case 32: handling->bRearLights = eVehicleLightsSize(atoi(word)); break;
					}
					field++;
				}
				gHandlingDataMgr.ConvertDataToGameUnits(handling);
			}
		}
	}
}

void LoadAdditionalVehicleColours(void)
{
	int fd;
	int i;
	char line[1024];
	int start, end;
	int section, numCols;
	enum {
		NONE,
		COLOURS,
		CARS
	};
	int r, g, b;
	char name[64];
	int colors[16];
	int n;

	if (!doesFileExist(".\\DATA\\CARCOLS_ADDITIONAL.DAT")) {
		return;
	}

	CFileMgr::SetDir("DATA");
	fd = CFileMgr::OpenFile("CARCOLS_ADDITIONAL.DAT", "r");
	CFileMgr::SetDir("");

	section = 0;
	numCols = 0;
	while (numCols < 256) {
		if (CVehicleModelInfo::ms_colourTextureTable[numCols].a == 0) {
			break;
		}
		numCols++;
	}
	while (CFileMgr::ReadLine(fd, line, sizeof(line))) {
		// find first valid character in line
		for (start = 0; ; start++)
			if (line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
				break;
		// find end of line
		for (end = start; ; end++) {
			if (line[end] == '\0' || line[end] == '\n')
				break;
			if (line[end] == ',' || line[end] == '\r')
				line[end] = ' ';
		}
		line[end] = '\0';

		// empty line
		if (line[start] == '#' || line[start] == '\0')
			continue;

		if (section == NONE) {
			if (line[start] == 'c' && line[start + 1] == 'o' && line[start + 2] == 'l')
				section = COLOURS;
			if (line[start] == 'c' && line[start + 1] == 'a' && line[start + 2] == 'r')
				section = CARS;
		}
		else if (line[start] == 'e' && line[start + 1] == 'n' && line[start + 2] == 'd') {
			section = NONE;
		}
		else if (section == COLOURS) {
			sscanf(&line[start],	// BUG: games doesn't add start
				"%d %d %d", &r, &g, &b);

			CVehicleModelInfo::ms_colourTextureTable[numCols].r = r;
			CVehicleModelInfo::ms_colourTextureTable[numCols].g = g;
			CVehicleModelInfo::ms_colourTextureTable[numCols].b = b;
			CVehicleModelInfo::ms_colourTextureTable[numCols].a = 0xFF;
			numCols++;
		}
		else if (section == CARS) {
			n = sscanf(&line[start],	// BUG: games doesn't add start
				"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				name,
				&colors[0], &colors[1],
				&colors[2], &colors[3],
				&colors[4], &colors[5],
				&colors[6], &colors[7],
				&colors[8], &colors[9],
				&colors[10], &colors[11],
				&colors[12], &colors[13],
				&colors[14], &colors[15]);
			CVehicleModelInfo* mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(name, NULL);
			assert(mi);
			mi->m_nNumColorVariations = (n - 1) / 2;
			for (i = 0; i < mi->m_nNumColorVariations; i++) {
				mi->m_anPrimaryColors[i] = colors[i * 2 + 0];
				mi->m_anSecondaryColors[i] = colors[i * 2 + 1];
			}
		}
	}

	CFileMgr::CloseFile(fd);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	CVector* pos;
	vec3df playerPos, soundPos, soundVel, dir;
	string key;
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
		Opcodes::RegisterOpcode(0x3F0A, oldReplaceTex);
		Opcodes::RegisterOpcode(0x3F0B, wheelSparks);
		Opcodes::RegisterOpcode(0x3F0C, setCarCollision);
		Opcodes::RegisterOpcode(0x3F0D, getDoorAngle);
		Opcodes::RegisterOpcode(0x3F0E, getWheelAngle);
		Opcodes::RegisterOpcode(0x3F0F, getWheelSpeed);
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
		Opcodes::RegisterOpcode(0x3F1A, getGasPedalAudio);
		Opcodes::RegisterOpcode(0x3F1B, getCarComponentPosition);
		Opcodes::RegisterOpcode(0x3F1C, getCarComponentRotation);
		Opcodes::RegisterOpcode(0x3F1D, getCarComponentOffset);
		Opcodes::RegisterOpcode(0x3F1E, setWheelAngle);
		Opcodes::RegisterOpcode(0x3F1F, setWheelSpeed);
		Opcodes::RegisterOpcode(0x3F20, getCarOrientation);
		Opcodes::RegisterOpcode(0x3F21, setCarOrientation);
		Opcodes::RegisterOpcode(0x3F22, popWheelie);
		Opcodes::RegisterOpcode(0x3F23, setRemote);
		Opcodes::RegisterOpcode(0x3F24, removeRemote);
		Opcodes::RegisterOpcode(0x3F25, applyForwardForce);
		Opcodes::RegisterOpcode(0x3F26, applyUpwardForce);
		Opcodes::RegisterOpcode(0x3F27, getWheelStatus);
		Opcodes::RegisterOpcode(0x3F28, setWheelStatus);
		Opcodes::RegisterOpcode(0x3F29, createLight);
		Opcodes::RegisterOpcode(0x3F2A, inRemote);
		Opcodes::RegisterOpcode(0x3F2B, attachVehicle);
		Opcodes::RegisterOpcode(0x3F2C, detachVehicle);
		Opcodes::RegisterOpcode(0x3F2D, setCarEngineSound);
		Opcodes::RegisterOpcode(0x3F2E, getDriveWheelsOnGround);
		Opcodes::RegisterOpcode(0x3F2F, getCarRotation);
		Opcodes::RegisterOpcode(0x3F30, rotateCar);
		Opcodes::RegisterOpcode(0x3F31, getRotationMatrix);
		Opcodes::RegisterOpcode(0x3F32, setRotationMatrix);
		Opcodes::RegisterOpcode(0x3F33, getVelocityDirection);
		Opcodes::RegisterOpcode(0x3F34, getVelocityVector);
		Opcodes::RegisterOpcode(0x3F35, setVelocityVector);
		Opcodes::RegisterOpcode(0x3F36, getSteeringAngle);
		Opcodes::RegisterOpcode(0x3F37, addObjects);
		Opcodes::RegisterOpcode(0x3F38, removeObjects);
		Opcodes::RegisterOpcode(0x3F39, getVelocity);
		Opcodes::RegisterOpcode(0x3F3A, setVelocity);
		Opcodes::RegisterOpcode(0x3F3B, getForwardVelocityVectorWithSpeed);
		Opcodes::RegisterOpcode(0x3F3C, getRelativeVelocity);
		Opcodes::RegisterOpcode(0x3F3D, skiMode);
		Opcodes::RegisterOpcode(0x3F3E, isWheelsOnGround);
		Opcodes::RegisterOpcode(0x3F3F, isWheelsNotOnGround);
		Opcodes::RegisterOpcode(0x3F40, isCarComponentVisible);
		Opcodes::RegisterOpcode(0x3F41, isCarComponentIndexVisible);
		Opcodes::RegisterOpcode(0x3F42, getAtMatrix);
		Opcodes::RegisterOpcode(0x3F43, getRightMatrix);
		Opcodes::RegisterOpcode(0x3F44, getUpMatrix);
		Opcodes::RegisterOpcode(0x3F45, rotateBonnet);
		Opcodes::RegisterOpcode(0x3F46, rotateBoot);
		Opcodes::RegisterOpcode(0x3F47, getCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3F48, getCarComponentIndexAlpha);
		Opcodes::RegisterOpcode(0x3F49, setCarComponentColor);
		Opcodes::RegisterOpcode(0x3F4A, setCarComponentIndexColor);
		Opcodes::RegisterOpcode(0x3F4B, updateHandling);
		Opcodes::RegisterOpcode(0x3F4C, setBuildingComponentVisibility);
		Opcodes::RegisterOpcode(0x3F50, isCarComponentNotVisible);
		Opcodes::RegisterOpcode(0x3F51, isCarComponentIndexNotVisible);
		Opcodes::RegisterOpcode(0x3F52, fadeCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3F53, fadeCarComponentIndexAlpha);
		Opcodes::RegisterOpcode(0x3F54, carComponentDigitOff);
		Opcodes::RegisterOpcode(0x3F55, carComponentDigitOn);
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
		Opcodes::RegisterOpcode(0x3F97, setFrequency);
		Opcodes::RegisterOpcode(0x3F98, setVolume);
		Opcodes::RegisterOpcode(0x3F99, setDoppler);
		Opcodes::RegisterOpcode(0x3F9A, setCarStatus);
		Opcodes::RegisterOpcode(0x3F9B, getCarStatus);
		Opcodes::RegisterOpcode(0x3F9C, setCarLights);
		Opcodes::RegisterOpcode(0x3F9D, getCarLights);


		//Opcodes::RegisterOpcode(0x3F37, replaceTex);
		//Opcodes::RegisterOpcode(0x3F38, addCompAnims);
		//Reserving 0x3F18-0x3F1F for get command

		Events::vehicleRenderEvent.before += [&](CVehicle* vehicle) {
			for (auto const& [vehicle_id, attachment] : carAttachments) {
				if (attachment.attached == vehicle) {
					CMatrix matrix = attachment.vehicle->m_placement * attachment.offset;
					attachment.attached->m_placement.up = matrix.up;
					attachment.attached->m_placement.at = matrix.at;
					attachment.attached->m_placement.right = matrix.right;
					attachment.attached->m_placement.pos = matrix.pos;
					attachment.attached->m_vecMoveSpeed = attachment.vehicle->m_vecMoveSpeed;
				}
			}
		};
		Events::initGameEvent += [] {
			//patch::Nop(0x58E59B, 5, true);
			//patch::Nop(0x58E611, 5, true);

			if (!loadedSound) {
				m_soundEngine = createIrrKlangDevice();
				m_soundEngine->setRolloffFactor(1.5f);
				m_soundEngine->setDopplerEffectParameters(2.0f, 10.0f);
				loadedSound = true;
			}
			else {
				m_soundEngine->removeAllSoundSources();
			}
			soundMap.clear();
			handlingData.clear();
			bikeHandlingData.clear();
			boatHandlingData.clear();
			carAttachments.clear();
			flyingHandlingData.clear();
			carAttachments.clear();
			ideMap.clear();
			removeObjectQueue.clear();
			LoadAdditionalHandlingData();
			LoadAdditionalVehicleColours();
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
							// Stop sound if wrecked
							int index = CPools::GetVehicleRef(soundMap[itr->first].vehicle);
							if (index < 0 || soundMap[itr->first].vehicle->m_nState == 5) {
								soundMap[itr->first].sound->stop();
								soundMap[itr->first].sound->drop();
								itr = soundMap.erase(itr);
								continue;
							}
							// Attach sound to vehicle
							Command<Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(soundMap[itr->first].vehicle, soundMap[itr->first].offset.x, soundMap[itr->first].offset.y, soundMap[itr->first].offset.z, &soundPos.X, &soundPos.Y, &soundPos.Z);
							soundPos.Y *= -1.0;
							soundMap[itr->first].sound->setPosition(soundPos);
							// Set speed for doppler effect
							soundVel.X = soundMap[itr->first].vehicle->m_vecMoveSpeed.x;
							soundVel.Y = soundMap[itr->first].vehicle->m_vecMoveSpeed.y * -1.0f;
							soundVel.Z = soundMap[itr->first].vehicle->m_vecMoveSpeed.z;
							soundMap[itr->first].sound->setVelocity(soundVel);
						}
						else {
							// Set sound to specified location
							soundPos.X = soundMap[itr->first].offset.x;
							soundPos.Y = soundMap[itr->first].offset.y;
							soundPos.Z = soundMap[itr->first].offset.z;
						}

						// Mute sound if > 150 units away, otherwise play at full volume
						/*distance = (float)playerPos.getDistanceFrom(soundPos);
						if (distance < 150.0f || !soundMap[itr->first].spatial) {
							soundMap[itr->first].sound->setVolume(1.0f);
						}
						else {
							soundMap[itr->first].sound->setVolume(0.0f);
						}*/
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

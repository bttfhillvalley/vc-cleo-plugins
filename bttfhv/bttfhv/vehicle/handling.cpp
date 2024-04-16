#define _USE_MATH_DEFINES
#include "CModelInfo.h"
#include "CDamageManager.h"

#include "../configuration/fileloader.h"
#include "../constants.h"
#include "../utils/math.h"
#include "handling.h"
#include <iostream>

using namespace std;

enum eDoorStatus
{
	DOOR_STATUS_OK,
	DOOR_STATUS_SMASHED,
	DOOR_STATUS_SWINGING,
	DOOR_STATUS_MISSING
};

map<int, int> handlingOverride = {
	{237, KURUMA_ID},
	{238, AIRTRAIN_ID},
	{239, AIRTRAIN_ID}
};

void DoorInit(CDoor* door, float minAngle, float maxAngle, uint8_t dir, uint8_t axis) {
	door->fAngleInPosTwo = minAngle;
	door->fAngleInPosOne = maxAngle;
	door->nAxisDirection = dir;
	door->nAxis = axis;
}

void UpdateHandling() {
	cout << "Updating handling" << endl;
	for (auto modelInfo : CModelInfo::ms_vehicleModelStore->m_sObject) {
		string name(modelInfo.m_szName);
		cout << name << endl;
		if (!name.empty() && handlingData.contains(name)) {
			cout << "Found vehicle: " << name << endl;
			cout << "Updating handling id: " << modelInfo.m_nHandlingId << endl;
			gHandlingDataMgr.m_aVehicleHandling[modelInfo.m_nHandlingId] = *handlingData[name];
		}
	}
}

void UpdateHandling(CVehicle *vehicle) {
	CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
	CVehicleModelInfo* modelInfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));

	string name(modelInfo->m_szName);
	if (!name.empty()) {
		if (handlingData.contains(name)) {
			auto handlingId = handlingOverride.find(vehicle->m_nModelIndex);
			if (handlingId != handlingOverride.end()) {
				gHandlingDataMgr.m_aVehicleHandling[handlingId->second] = *handlingData[name];
			}
			vehicle->m_pHandlingData = handlingData[name];
			automobile->m_fMass = automobile->m_pHandlingData->fMass;
			automobile->m_fTurnMass = automobile->m_pHandlingData->fTurnMass;
			automobile->m_vecCentreOfMass = automobile->m_pHandlingData->m_vecCentreOfMass;
			automobile->SetupSuspensionLines();
			automobile->m_nVehicleFlags.bIsVan = !!(handlingData[name]->uFlags & HANDLING_IS_VAN);
			automobile->m_nVehicleFlags.bHideOccupants = !!(handlingData[name]->uFlags & HANDLING_IS_BUS);  // Plugin-sdk is named wrong
			automobile->m_nVehicleFlags.bIsBus = !!(handlingData[name]->uFlags & HANDLING_IS_BIG);  // Plugin-sdk is named wrong
			automobile->m_nVehicleFlags.bIsBig = !!(handlingData[name]->uFlags & HANDLING_IS_LOW);  // Plugin-sdk is named wrong
			if (handlingData[name]->uFlags & HANDLING_REV_BONNET)
				DoorInit(&automobile->m_aDoors[BONNET], -M_PI * 0.3f, 0.0f, 1, 0);
			else
				DoorInit(&automobile->m_aDoors[BONNET], 0.0f, M_PI * 0.3f, 1, 0);
			if (automobile->m_pHandlingData->uFlags & HANDLING_NO_DOORS) {
				automobile->m_carDamage.SetDoorStatus(DOOR_FRONT_LEFT, DOOR_STATUS_MISSING);
				automobile->m_carDamage.SetDoorStatus(DOOR_FRONT_RIGHT, DOOR_STATUS_MISSING);
				automobile->m_carDamage.SetDoorStatus(DOOR_REAR_LEFT, DOOR_STATUS_MISSING);
				automobile->m_carDamage.SetDoorStatus(DOOR_REAR_RIGHT, DOOR_STATUS_MISSING);
			}
		}
	}
}

void UpdateFlyingHandling(CVehicle* vehicle) {
	CVehicleModelInfo* modelInfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));

	string name(modelInfo->m_szName);
	if (!name.empty()) {
		if (vehicle->m_pFlyingHandling == &gHandlingDataMgr.m_aFlyingHandling[0] && flyingHandlingData.contains(name)) {
			vehicle->m_pFlyingHandling = flyingHandlingData[name];
		}
	}
}

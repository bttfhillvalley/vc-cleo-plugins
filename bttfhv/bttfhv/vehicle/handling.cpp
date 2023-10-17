#include "CModelInfo.h"

#include "../configuration/fileloader.h"
#include "../constants.h"
#include "handling.h"

using namespace std;

map<int, int> handlingOverride = {
	{237, KURUMA_ID},
	{238, AIRTRAIN_ID},
	{239, AIRTRAIN_ID}
};

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
			automobile->SetupSuspensionLines();
			automobile->m_nVehicleFlags.bIsVan = !!(handlingData[name]->uFlags & HANDLING_IS_VAN);
			automobile->m_nVehicleFlags.bHideOccupants = !!(handlingData[name]->uFlags & HANDLING_IS_BUS);  // Plugin-sdk is named wrong
			automobile->m_nVehicleFlags.bIsBus = !!(handlingData[name]->uFlags & HANDLING_IS_BIG);  // Plugin-sdk is named wrong
			automobile->m_nVehicleFlags.bIsBig = !!(handlingData[name]->uFlags & HANDLING_IS_LOW);  // Plugin-sdk is named wrong
		}
		if (flyingHandlingData.contains(name)) {
			vehicle->m_pFlyingHandling = flyingHandlingData[name];
		}
		else {
			vehicle->m_pFlyingHandling = flyingHandlingData["dmc12"];
		}
	}
}
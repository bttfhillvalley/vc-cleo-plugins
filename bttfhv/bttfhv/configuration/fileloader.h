#pragma once
#include <map>
#include <set>
#include "plugin.h"
#include "cHandlingDataMgr.h"


using namespace std;
extern map<string, tHandlingData*> handlingData;
extern map<string, tFlyingHandlingData*> flyingHandlingData;
extern map<string, tBoatHandlingData*> boatHandlingData;
extern map<string, tBikeHandlingData*> bikeHandlingData;
extern map<string, int> keyMap;


void InitializeKeyMap(void);
void LoadKeyConfig(void);
void LoadAdditionalHandlingData(void);
void LoadAdditionalVehicleColours(void);
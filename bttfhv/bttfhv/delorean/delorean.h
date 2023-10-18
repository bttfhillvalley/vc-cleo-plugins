#pragma once
#include <map>
#include "plugin.h"
#include "eEntityStatus.h"

#include "enums.h"
#include "components.h"
#include "options.h"

#define GAME_SPEED_TO_METERS_PER_SECOND 50.0f

using namespace std;

class Delorean
{
public:
	int vehicleHandle;

	CAutomobile* timeMachine;

	Delorean(CVehicle* vehicle);

	bool IsWrecked() { return timeMachine->m_nState == STATUS_WRECKED; }
	float GetSpeed() { return timeMachine->GetSpeed().Magnitude() * GAME_SPEED_TO_METERS_PER_SECOND; }
	void Setup();
	void SetupGlow();
	void SetupPlutoniumBox();
	void SetupShifter();
	void SetupSid();
	void SetupSpeedo();
	void SetupTimeCircuits();
	void ShowStock();
	void SetComponentVisibility(const vector<string>& components, int visible, string prefix);
	void ShowComponents(const vector<string>& components);
	void HideComponents(const vector<string>& components);
	void ShowHoverComponents(const vector<string>& components);
	void HideHoverComponents(const vector<string>& components);
	void ShowOption(const map<int, vector<string>>& options, int option);
	void ShowHoverOption(const map<int, vector<string>>& options, int option);
	void HideAllOptions(const map<int, vector<string>>& options);
	void HideAllHoverOptions(const map<int, vector<string>>& options);

	void AnimateDoorStruts();
	void AnimateShifter();

	void Update();
};

extern map<CVehicle*, Delorean*> deloreanMap;
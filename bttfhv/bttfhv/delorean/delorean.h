#pragma once
#include <map>
#include "plugin.h"
#include "eEntityStatus.h"

#include "enums.h"
#include "components.h"
#include "options.h"

using namespace std;

class Delorean
{
public:
	int vehicleHandle;

	CAutomobile* timeMachine;

	Delorean(CVehicle* vehicle);

	bool IsWrecked() { return timeMachine->m_nState == STATUS_WRECKED; }
	void SetGlow();
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
};

extern map<CVehicle*, Delorean*> deloreanMap;
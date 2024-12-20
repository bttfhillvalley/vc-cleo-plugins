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
private:
	vector<RwFrame*> bonnetFrames;
	vector<RwFrame*> bootFrames;
	vector<RwFrame*> doorLeftFrontFrames;
	vector<RwFrame*> doorRightFrontFrames;
	vector<RwFrame*> bumpFrontFrames;
	vector<RwFrame*> bumpRearFrames;
	vector<RwFrame*> windscreenFrames;
	vector<RwFrame*> wingLeftFrontFrames;
	vector<RwFrame*> wingLeftRearFrames;
	vector<RwFrame*> wingRightFrontFrames;
	vector<RwFrame*> wingRightRearFrames;
	vector<RwFrame*> GetFrames(vector<string> components);
	bool framesLoaded;

	void LoadFrames();
	void processDoorDamage(eDoors door, vector<RwFrame*> *frames);
	void processPanelDamage(ePanels panel, vector<RwFrame*> *frames);
public:
	int vehicleHandle;

	CAutomobile* automobile;
	unsigned char leftDoor;
	unsigned char rightDoor;
	unsigned char bonnetState;
	unsigned char bootState;
	float rpmNeedle = 0.0f;
	unsigned int doorStatus[6];
	unsigned int panelStatus[7];
	float wheelRot[4];
	float wheelSpeed[4];

	// Component states
	uint8_t bodyType : 1;
	uint8_t driveTrain : 1;
	uint8_t wheelType : 2;
	uint8_t rearDeck : 2;
	uint8_t plateType : 2;
	uint8_t hoodType : 1;
	uint8_t hitch : 1;
	uint8_t hookType : 2;
	uint8_t bulovaClock : 1;
	uint8_t fireboxGauge : 1;

	Delorean(CVehicle* vehicle);

	bool IsWrecked() { return automobile->m_nState == STATUS_WRECKED; }
	float GetSpeed() { return automobile->GetSpeed().Magnitude() * GAME_SPEED_TO_METERS_PER_SECOND; }
	void Setup();
	void SetupGlow();
	void SetupPlutoniumBox();
	void SetupShifter();
	void SetupSid();
	void SetupSpeedo();
	void SetupTimeCircuits();
	void SetupWormhole();
	void ShowStock();
	void SetComponentVisibility(const vector<string>& components, int visible, string prefix);
	void ShowComponents(const vector<string>& components);
	void HideComponents(const vector<string>& components);
	void ShowOption(const map<int, vector<string>>& options, int option);
	void HideAllOptions(const map<int, vector<string>>& options);
	bool IsTimeMachine();

	void HandleBonnet();
	void HandleBoot();

	void ProcessDoor();
	void ProcessBonnet();
	void ProcessShifter();
	void ProcessDamage();
	void ProcessVariation();
	void ProcessWheels();
	CObject* SpawnFlyingComponent(RwFrame* frame, unsigned int type);

	void Update();
};

extern map<CVehicle*, Delorean*> deloreanMap;
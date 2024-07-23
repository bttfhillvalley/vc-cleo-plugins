#pragma once
#include <map>
#include "plugin.h"
#include "eEntityStatus.h"

using namespace std;

#define LUXOR_FRONT_OFFSET -0.457479f
#define LUXOR_BACK_OFFSET -0.457285f

enum FLYING_STATUS {
	STATUS_LANDED,
	STATUS_TAKING_OFF,
	STATUS_FLYING,
	STATUS_LANDING,
};


class Luxor
{
public:
	int vehicleHandle;
	unsigned char flying;
	bool keyDown;
	int flyingFrame;
	float wheelRot[4];
	float wheelSpeed[4];
	int wheelStatus[4];
	bool wheelsDown;

	CAutomobile* automobile;

	Luxor(CVehicle* vehicle);

	bool IsWrecked() { return automobile->m_nState == STATUS_WRECKED; }
	void Setup();
	void HandleHover();
	void UpdateHover();
	void UpdateWheels();
	void Update();
private:
	void moveSuspension(string wheel, float offset);
	float getHoverDamper();
};

extern map<CVehicle*, Luxor*> luxorMap;
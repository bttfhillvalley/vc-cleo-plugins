#include "CDamageManager.h"
#include "CVehicleModelInfo.h"
#include "CWorld.h"
#include "../constants.h"
#include "../utils/math.h"
#include "../sound/sound.h"
#include "../vehicle/components.h"
#include "../vehicle/handling.h"
#include <iostream>

#include "delorean.h"

map<CVehicle*, Delorean*> deloreanMap;

Delorean::Delorean(CVehicle* vehicle) {
	timeMachine = reinterpret_cast<CAutomobile*>(vehicle);
	Setup();
	ShowStock();
}

void Delorean::ShowStock() {
	ShowComponents(BRAKE_COMPONENTS);
	HideHoverComponents(BRAKE_COMPONENTS);
	HideHoverComponents(THRUSTER_COMPONENTS);
	HideAllHoverOptions(WHEEL_OPTIONS);

	ShowOption(WHEEL_OPTIONS, WHEEL_STOCK);
	ShowOption(BODY_OPTIONS, BODY_STOCK);
	ShowOption(GRILL_HITCH_OPTIONS, GRILL_HITCH_STOCK);
	HideAllOptions(GRILL_BRACKET_OPTIONS);
	HideAllOptions(REAR_DECK_OPTIONS);

	HideAllOptions(PLATE_OPTIONS);
	ShowOption(HOOD_OPTIONS, HOOD_STOCK);
	ShowOption(HITCH_OPTIONS, HITCH_NONE);
	ShowOption(HOOK_OPTIONS, HOOK_NONE);
	ShowOption(DRIVETRAIN_OPTIONS, DRIVETRAIN_STOCK);
	ShowOption(BULOVA_CLOCK_OPTIONS, BULOVA_CLOCK_NONE);
	ShowOption(FIREBOX_GAUGE_OPTIONS, FIREBOX_GAUGE_NONE);

	HideComponents(PLUTONIUM_COMPONENTS);
}

void Delorean::Setup() {
	SetupGlow();
	SetupSid();
	SetupPlutoniumBox();
	SetupShifter();
	SetupSpeedo();
	SetupTimeCircuits();
	SetupWormhole();
	UpdateHandling(timeMachine);
}

void Delorean::SetupGlow() {
	for (const auto& component : GLOWING_COMPONENTS) {
		setGlow(timeMachine, component, 1);
	}

	for (const auto& component : GLOWING_HIDDEN_COMPONENTS) {
		setGlow(timeMachine, component, 1);
		setVisibility(timeMachine, component, 0);
	}
}

void Delorean::SetupPlutoniumBox() {
	for (int index = 1; index <= 12; index++) {
		setVisibility(timeMachine, getComponentIndex("plutcan", index), 0);
		setVisibility(timeMachine, getComponentIndex("plutcanliquid", index), 0);
		setVisibility(timeMachine, getComponentIndex("plutcaninterior", index), 0);
		setVisibility(timeMachine, getComponentIndex("plut", index), 0);
	}
}

void Delorean::SetupSpeedo() {
	for (int index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(timeMachine, "digitalspeedodigit", index);
	}
}

void Delorean::SetupWormhole() {
	for (int index = 1; index <= 70; index++) {
		SetGlowAndHideIndex(timeMachine, "wormhole", index);
		SetGlowAndHideIndex(timeMachine, "wormholer", index);
	}
}

void Delorean::SetComponentVisibility(const vector<string>& components, int visible, string prefix = "") {
	for (string name : components) {
		setVisibility(timeMachine, (prefix + name).c_str(), visible);
	}
}

void Delorean::ShowHoverComponents(const vector<string>& components) {
	SetComponentVisibility(components, 1, "fx");
}

void Delorean::HideHoverComponents(const vector<string>& components) {
	SetComponentVisibility(components, 0, "fx");
}

void Delorean::ShowComponents(const vector<string>& components) {
	SetComponentVisibility(components, 1);
}

void Delorean::HideComponents(const vector<string>& components) {
	SetComponentVisibility(components, 0);
}

void Delorean::ShowOption(const map<int, vector<string>>& options, int option) {
	vector<string> showOption;
	for (const auto& it : options) {
		if (it.first == option) {
			showOption = it.second;
		} else {
			HideComponents(it.second);
		}
	}
	ShowComponents(showOption);
}

void Delorean::ShowHoverOption(const map<int, vector<string>>& options, int option) {
	vector<string> showOption;
	for (const auto& it : options) {
		if (it.first == option) {
			showOption = it.second;
		}
		else {
			HideHoverComponents(it.second);
		}
	}
	ShowHoverComponents(showOption);
}

void Delorean::HideAllOptions(const map<int, vector<string>>& options) {
	for (const auto& it : options) {
		HideComponents(it.second);
	}
}

void Delorean::HideAllHoverOptions(const map<int, vector<string>>& options) {
	for (const auto& it : options) {
		HideHoverComponents(it.second);
	}
}

bool Delorean::IsTimeMachine() {
	return getVisibility(timeMachine, "bttf1") || getVisibility(timeMachine, "bttf2");
}

void Delorean::Update() {
	HandleBonnet();
	HandleBoot();
	ProcessDoor();
	ProcessBonnet();
	ProcessShifter();
}
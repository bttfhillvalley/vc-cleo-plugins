#include "CDamageManager.h"
#include "CVehicleModelInfo.h"
#include "CWorld.h"
#include "../constants.h"
#include "../rw/utils.h"
#include "../utils/math.h"
#include "../sound/sound.h"
#include "../vehicle/components.h"
#include "../vehicle/handling.h"
#include <iostream>

#include "delorean.h"

map<CVehicle*, Delorean*> deloreanMap;

Delorean::Delorean(CVehicle* vehicle) {
	automobile = reinterpret_cast<CAutomobile*>(vehicle);
	Setup();
	ShowStock();
}

void Delorean::ShowStock() {
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
	HideComponents(THRUSTER_COMPONENTS);
	HideComponents(FROSTED_COMPONENTS);

	// HACK: This allows CLEO to hook into the model by showing both underbody models
	setVisibility(automobile, "underbodybttf2", 1);
}

void Delorean::Setup() {
	int i;
	for (i = 0; i < 6; i++) doorStatus[i] = 0;
	SetupGlow();
	SetupSid();
	SetupPlutoniumBox();
	SetupShifter();
	SetupSpeedo();
	SetupTimeCircuits();
	SetupWormhole();
	UpdateHandling(automobile);
}

void Delorean::SetupGlow() {
	for (const auto& component : GLOWING_COMPONENTS) {
		setGlow(automobile, component, 1);
	}

	for (const auto& component : GLOWING_HIDDEN_COMPONENTS) {
		setGlow(automobile, component, 1);
		setVisibility(automobile, component, 0);
	}
}

void Delorean::SetupPlutoniumBox() {
	for (int index = 1; index <= 12; index++) {
		setVisibility(automobile, getComponentIndex("plutcan", index), 0);
		setVisibility(automobile, getComponentIndex("plutcanliquid", index), 0);
		setVisibility(automobile, getComponentIndex("plutcaninterior", index), 0);
		setVisibility(automobile, getComponentIndex("plut", index), 0);
	}
}

void Delorean::SetupSpeedo() {
	for (int index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(automobile, "digitalspeedodigit", index);
	}
}

void Delorean::SetupWormhole() {
	for (int index = 1; index <= 70; index++) {
		SetGlowAndHideIndex(automobile, "wormhole", index);
		SetGlowAndHideIndex(automobile, "wormholer", index);
	}
}

void Delorean::SetComponentVisibility(const vector<string>& components, int visible, string prefix = "") {
	for (string name : components) {
		setVisibility(automobile, (prefix + name).c_str(), visible);
	}
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

void Delorean::HideAllOptions(const map<int, vector<string>>& options) {
	for (const auto& it : options) {
		HideComponents(it.second);
	}
}

bool Delorean::IsTimeMachine() {
	return getVisibility(automobile, "bttf1") || getVisibility(automobile, "bttf2");
}

void Delorean::Update() {
	HandleBonnet();
	HandleBoot();
	ProcessDoor();
	ProcessBonnet();
	ProcessShifter();
	ProcessDamage();
}
#include "CDamageManager.h"
#include "CVehicleModelInfo.h"
#include "CWorld.h"
#include "../constants.h"
#include "../rw/utils.h"
#include "../utils/math.h"
#include "../sound/sound.h"
#include "../vehicle/components.h"
#include "../vehicle/handling.h"
#include "../vehicle/hover.h"
#include <iostream>

#include "delorean.h"

map<CVehicle*, Delorean*> deloreanMap;

Delorean::Delorean(CVehicle* vehicle) {
	automobile = reinterpret_cast<CAutomobile*>(vehicle);
	Setup();
	ShowStock();
}

void Delorean::ShowStock() {
	bodyType = BODY_STOCK;
	driveTrain = DRIVETRAIN_STOCK;
	wheelType = WHEEL_STOCK;
	rearDeck = REAR_DECK_NONE;
	plateType = PLATE_NONE;
	hoodType = HOOD_STOCK;
	hitch = HITCH_NONE;
	hookType = HOOK_NONE;
	bulovaClock = BULOVA_CLOCK_NONE;
	fireboxGauge = FIREBOX_GAUGE_NONE;

	ShowOption(BODY_OPTIONS, bodyType);
	ShowOption(DRIVETRAIN_OPTIONS, driveTrain);
	ShowOption(WHEEL_OPTIONS, wheelType);
	ShowOption(REAR_DECK_OPTIONS, rearDeck);
	ShowOption(PLATE_OPTIONS, plateType);
	ShowOption(HOOD_OPTIONS, hoodType);
	ShowOption(HITCH_OPTIONS, hitch);
	ShowOption(HOOK_OPTIONS, hookType);
	ShowOption(BULOVA_CLOCK_OPTIONS, bulovaClock);
	ShowOption(FIREBOX_GAUGE_OPTIONS, fireboxGauge);

	HideComponents(PLUTONIUM_COMPONENTS);
	HideComponents(THRUSTER_COMPONENTS);
	HideComponents(FROSTED_COMPONENTS);
}

void Delorean::Setup() {
	int i;
	framesLoaded = false;
	for (i = 0; i < 6; i++) doorStatus[i] = 0;
	for (i = 0; i < 7; i++) panelStatus[i] = 0;
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

void Delorean::ProcessVariation() {
	// Body Type
	if (getVisibility(automobile, "bttfparts")) {
		bodyType = BODY_TIME_MACHINE;
	}
	else {
		bodyType = BODY_STOCK;
	}

	// Drivetrain
	if (getVisibility(automobile, "underbodybttf2")) {
		driveTrain = DRIVETRAIN_HOVER;
	}
	else {
		driveTrain = DRIVETRAIN_STOCK;
	}

	// Wheels
	if (getVisibility(automobile, "wheelbttf3lb") || getVisibility(automobile, "fxwheelbttf3lb")) {
		wheelType = WHEEL_WHITEWALLS;
	}
	else if (getVisibility(automobile, "wheelbttf3rrlb") || getVisibility(automobile, "fxwheelbttf3rrlb")) {
		wheelType = WHEEL_RAILROAD;
	}
	else {
		wheelType = WHEEL_STOCK;
	}

	// Rear Deck
	if (getVisibility(automobile, "bttf1")) {
		rearDeck = REAR_DECK_PLUTONIUM;
	}
	else if (getVisibility(automobile, "bttf2")) {
		rearDeck = REAR_DECK_FUSION;
	}
	else {
		rearDeck = REAR_DECK_NONE;
	}

	// Plate
	if (getVisibility(automobile, "platestock")) {
		plateType = PLATE_STOCK;
	}
	else if (getVisibility(automobile, "plate")) {
		plateType = PLATE_OUTATIME;
	}
	else if (getVisibility(automobile, "platebttf2")) {
		plateType = PLATE_BARCODE;
	}
	else {
		plateType = PLATE_NONE;
	}

	// Hook
	if (getVisibility(automobile, "holderbttf1") && !getVisibility(automobile, "hookbttf1")) {
		hookType = HOOK_HOLDER;
	}
	else if (getVisibility(automobile, "hookcablesoffbttf1")) {
		hookType = HOOK_SIDE;
	}
	else if (getVisibility(automobile, "hookcablesonbttf1")) {
		hookType = HOOK_UP;
	}
	else {
		hookType = HOOK_NONE;
	}

	// Hood type
	if (getVisibility(automobile, "bonnetbttf3")) {
		hoodType = HOOD_HOODBOX;
	}
	else {
		hoodType = HOOD_STOCK;
	}

	// Hitch
	if (getVisibility(automobile, "hitch")) {
		hitch = HITCH_ATTACHED;
	}
	else {
		hitch = HITCH_NONE;
	}

	// Bulova Clock
	if (getVisibility(automobile, "clock")) {
		bulovaClock = BULOVA_CLOCK_ATTACHED;
	}
	else {
		bulovaClock = BULOVA_CLOCK_NONE;
	}

	// Firebox Gauge
	if (getVisibility(automobile, "fireboxgauge")) {
		fireboxGauge = FIREBOX_GAUGE_ATTACHED;
	}
	else {
		fireboxGauge = FIREBOX_GAUGE_NONE;
	}
}

void Delorean::Update() {
	HandleBonnet();
	HandleBoot();
	ProcessDoor();
	ProcessBonnet();
	ProcessShifter();
	ProcessDamage();
	ProcessVariation();
}
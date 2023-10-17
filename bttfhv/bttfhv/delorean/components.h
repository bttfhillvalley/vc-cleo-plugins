#pragma once
#include <map>
#include <vector>

using namespace std;

enum Plate {
	PLATE_STOCK,
	PLATE_OUTATIME,
	PLATE_BARCODE,
};

enum Hook {
	HOOK_NONE,
	HOOK_HOLDER,
	HOOK_SIDE,
	HOOK_UP,
};

enum Hood {
	HOOD_STOCK,
	HOOD_HOODBOX,
};

enum Hitch {
	HITCH_NONE,
	HITCH_ATTACHED,
};

enum DriveTrain {
	DRIVETRAIN_STOCK,
	DRIVETRAIN_HOVER,
};

enum Wheel {
	WHEEL_STOCK,
	WHEEL_WHITEWALLS,
	WHEEL_RAILROAD,
};

enum TimeMachine {
	TIME_MACHINE_NONE,
	TIME_MACHINE_ATTACHED,
};

enum GrillHitch {
	GRILL_HITCH_STOCK,
	GRILL_HITCH_TM,
};

const map<Plate, vector<string>> plateComponents = {
	{ PLATE_STOCK, {} },
	{ PLATE_OUTATIME, { "plate" } },
	{ PLATE_BARCODE, { "platebttf2" } },
};

const map<Hook, vector<string>> hookComponents = {
	{ HOOK_NONE, { "nohookbttf1" } },
	{ HOOK_HOLDER, { "holderbttf1" } },
	{ HOOK_SIDE, { "hookbttf1", "holderbttf1"} },
	{ HOOK_UP, { "hookandholderbttf1" } },
};

const map<Hood, vector<string>> hoodComponents = {
	{ HOOD_STOCK, {} },
	{ HOOD_HOODBOX, { "bonnetbttf3" } },
};

const map<Hitch, vector<string>> hitchComponents = {
	{ HITCH_NONE, {} },
	{ HITCH_ATTACHED, { "hitch" } },
};


const map<TimeMachine, vector<string>> timeMachineComponents = {
	{ TIME_MACHINE_NONE, {
		"gloveboxclosed",
		"cargonet",
		"consoletop",
		"consoletopcontrols",
		"passengerseatbelt",
		"consoleacgrills",
		"enginecover",
		"boot_hi_ok",
		"windscreen_rearview",
		"wing_lr_hi_ok_glass",
		"wing_rr_hi_ok_glass",
		"back_glass",
		"platestock",
		"exhaustmodel",
	}},
	{ TIME_MACHINE_ATTACHED, {
		"bttfparts",
		"wing_lf_hi_ok_sh",
		"wing_rf_hi_ok_sh",
		"wing_lr_hi_ok_sh",
		"wing_rr_hi_ok_sh",
		"roof_coils_shadows",
		"interiorbttf",
		"wiresexterior",
		"wiresinterior",
		"fluxcoils",
		"fluxcoilsholders",
		"keychain",
		"consoletoptm",
		"wcontrollftm",
		"wcontrolrftm",
		"gloveboxgauges",
		"gloveboxgaugeslights",
		"pchamberemptylight",
		"gloveboxopen",
		"tcddisplay",
		"compass",
		"digitalspeedo",
		"tcdhandle",
		"tcdswitch",
		"stwheelbuttons",
		"stwheelbuttonslights",
		"fluxcapacitor",
		"fluxcapacitorglass",
		"fluxcapacitortubes",
		"bulkhead",
		"sid",
		"pchamberneedle",
		"pchamberrefneedle",
		"ppowerneedle",
		"primaryneedle",
		"overheadconsole",
		"overheadconsolelights",
		"tcdkeypadbutton0",
		"tcdkeypadbutton1",
		"tcdkeypadbutton2",
		"tcdkeypadbutton3",
		"tcdkeypadbutton4",
		"tcdkeypadbutton5",
		"tcdkeypadbutton6",
		"tcdkeypadbutton7",
		"tcdkeypadbutton8",
		"tcdkeypadbutton9",
		"tcdkeypadbuttonenter",
	}}
};

const map<GrillHitch, vector<string>> grillHitchComponents = {
	{ GRILL_HITCH_STOCK, {
		"bump_front_hi_ok_grill",
	}},
	{ GRILL_HITCH_TM, {
		"grillhitch",
		"bump_front_hi_ok_grilltm",
	}},
};

const string plutoniumChamberComponents[] = {
	"bttf1",
	"nohookbttf1",
	"reactorlidbttf1",
};

const string mrFusionComponents[] = {
	"bttf2",
	"mrfusion",
	"fusionlatch",
};

const string bulovaClockComponents[] = {
	"hourhand",
	"minutehand",
	"clockhammer",
	"clockhammer",
	"alarmhand",
	"clock",
};

const string fireboxGaugeComponents[] = {
	"fireboxgauge",
	"fireboxgaugeneedle",
};

const map<DriveTrain, vector<string>> driveTrainComponents{
	{ DRIVETRAIN_STOCK, {
		"frontsuspbttf1",
		"exhaustmodeltm",
		"xchassis",
		"underbody",
	}},
	{ DRIVETRAIN_HOVER, {
		// BTTF2 parts
		"xchassisbttf2",
		"underbodybttf2",

		// Suspension
		"strutrb",
		"holderrb",
		"shockpistonrb",
		"shocklb",

		"strutlb",
		"holderlb",
		"shockpistonlb",
		"shocklb",

		"strutrf",
		"holderrf",
		"shockpistonrf",
		"shockrf",

		"strutlf",
		"holderlf",
		"shockpistonlf",
		"shocklf",
	}}
};

const map<Wheel, vector<string>> wheelComponents = {
	{ WHEEL_STOCK, {
		"wheelbttf1rb",
		"tirebttf1rb",

		"wheelbttf1lb",
		"tirebttf1lb",

		"wheelbttf1rf",
		"tirebttf1rf",

		"wheelbttf1lf",
		"tirebttf1lf",
	}},
	{ WHEEL_WHITEWALLS, {
		"wheelbttf3rb",
		"tirebttf3rb",
		"hubcapbttf3rb",

		"wheelbttf3lb",
		"tirebttf3lb",
		"hubcapbttf3lb",

		"wheelbttf3rf",
		"tirebttf3rf",
		"hubcapbttf3rf",

		"wheelbttf3lf",
		"tirebttf3lf",
		"hubcapbttf3lf",
	}},
	{ WHEEL_RAILROAD, {
		"wheelbttf3rrrb",
		"wheelbttf3rrlb",
		"wheelbttf3rrrf",
		"wheelbttf3rrlf",
	}}
};

const string plutoniumComponents[] = {
	"plutcan"
	"plutcanliquid"
	"plutcaninterior"
	"plut"
	"pluttop"
	"plutbox"
};
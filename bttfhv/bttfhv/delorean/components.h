#pragma once
#include <string>
#include <tuple>
#include <vector>

using namespace std;

const vector<string> PLATE_OUTATIME_COMPONENTS = {
	"plate",
};

const vector<string> PLATE_BARCODE_COMPONENTS = {
	"platebttf2",
};

const vector<string> HOOK_NONE_COMPONENTS = {

};

const vector<string> HOOK_HOLDER_COMPONENTS = {
	"holderbttf1",
	"hookguidebttf1",
};

const vector<string> HOOK_SIDE_COMPONENTS = {
	"holderbttf1",
	"hookguidebttf1",
	"hookbttf1",
	"hookcableplugbttf1",
	"hookcablesoffbttf1",
};

const vector<string> HOOK_UP_COMPONENTS = {
	"holderbttf1",
	"hookguidebttf1",
	"hookbttf1",
	"hookcableplugbttf1",
	"hookcablesonbttf1",
};

const vector<string> HOODBOX_COMPONENTS = {
	"bonnetbttf3",
};

const vector<string> HITCH_COMPONENTS = {
	"hitch",
};

const vector<string> STOCK_COMPONENTS = {
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
};

const vector<string> TIME_MACHINE_COMPONENTS = {
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
	"overheadconsoleelighth",
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
};

const vector<string> GRILL_HITCH_STOCK_COMPONENTS = {

	"bump_front_hi_ok_grill",
};

const vector<string> GRILL_HITCH_TM_COMPONENTS = {
	"grillhitch",
	"bump_front_hi_ok_grilltm",
};

const vector<string> GRILL_BRACKET_STOCK_COMPONENTS = {
	"grillhitchbracketbttf1",
};

const vector<string> GRILL_BRACKET_HOVER_COMPONENTS = {
	"grillhitchbracketbttf2",
};

const vector<string> PLUTONIUM_CHAMBER_COMPONENTS = {
	"bttf1",
	"nohookbttf1",
	"reactorlidbttf1",
};

const vector<string> MR_FUSION_COMPONENTS = {
	"bttf2",
	"mrfusion",
	"fusionlatch",
};

const vector<string> BULOVA_CLOCK_COMPONENTS = {
	"hourhand",
	"minutehand",
	"clockhammer",
	"clockhammer",
	"alarmhand",
	"clock",
};

const vector<string> FIREBOX_GAUGE_COMPONENTS = {
	"fireboxgauge",
	"fireboxgaugeneedle",
};

const vector<string> DRIVETRAIN_STOCK_COMPONENTS = {
	"frontsuspbttf1",
	"exhaustmodeltm",
	"xchassis",
	"underbody",
	"bonnetanchorl",
	"bonnetanchorr",
};

const vector<string> DRIVETRAIN_HOVER_COMPONENTS = {
	// BTTF2 parts
	"xchassisbttf2",
	"underbodybttf2",
	"bonnetanchorlbttf2",
	"bonnetanchorrbttf2",

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
};

const vector<string> BRAKE_COMPONENTS = {
	"wheel_rb",
	"wheel_lb",
	"wheel_rf",
	"wheel_lf",
};

const vector<string> WHEEL_STOCK_COMPONENTS = {
	"wheelbttf1rb",
	"tirebttf1rb",

	"wheelbttf1lb",
	"tirebttf1lb",

	"wheelbttf1rf",
	"tirebttf1rf",

	"wheelbttf1lf",
	"tirebttf1lf",
};

const vector<string> WHEEL_WHITEWALLS_COMPONENTS = {
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
};

const vector<string> WHEEL_RAILROAD_COMPONENTS = {
	"wheelbttf3rrrb",
	"wheelbttf3rrlb",
	"wheelbttf3rrrf",
	"wheelbttf3rrlf",
};

const vector<string> THRUSTER_COMPONENTS = {
	"thrusterbttf2rb",
	"thrusterbttf2lb",
	"thrusterbttf2rf",
	"thrusterbttf2lf",
};

const vector<string> PLUTONIUM_COMPONENTS = {
	"plutcan",
	"plutcanliquid",
	"plutcaninterior",
	"plut",
	"pluttop",
	"plutbox",
};

const vector<string> GLOWING_COMPONENTS = {
	"lightFL",
	"lightFR",
	"door_lf_hi_ok_lights",
	"door_rf_hi_ok_lights",
	"doorlflights",
	"doorrflights",
	"brklights",
	"revlights",
	"rpmneedle",
	"voltsneedle",
	"oilneedle",
	"fuelneedle",
	"tempneedle",
	"speedoneedle",
};

const vector<string> GLOWING_HIDDEN_COMPONENTS = {
	"lowbeamslight",
	"highbeamslight",
	"turnsignalllight",
	"turnsignalrlight",
	"turnlightrb",
	"turnlightlb",
	"turnlightrf",
	"turnlightlf",
	"headlightshighon",
	"seatbeltlight",
	"lambdalight",
	"oillight",
	"doorajarlight",
	"fuellight",
	"brakelight",
	"batterylight",
	"pchamberemptylight",
	"stwheelbuttonslights",
	"overheadconsolelights",
	"overheadconsoleelight",
	"flux1",
	"flux2",
	"flux3",
	"flux4",
	"flux5",
	"flux6",
	"fluxcapacitorlightson",
	"fluxcoilson",
	"wormhole",
	"wormhole1",
	"fxthrusterbttf2rbon",
	"fxthrusterbttf2rbth",
	"fxthrusterbttf2rfon",
	"fxthrusterbttf2rfth",
	"fxthrusterbttf2lbon",
	"fxthrusterbttf2lbth",
	"fxthrusterbttf2lfon",
	"fxthrusterbttf2lfth",
	"inner_vents",
	"inner_ventsglow",
	"bottomlights",
	"chaserlights1",
	"chaserlights2",
	"chaserlights3",
	"chaserlights4",
	"chaserlights5",
	"tcdswitchlighton",
	"tcdswitchlightoff",
	"tcdkeypadlightson",
	"tcdkeypadenterlighton",
	"gloveboxgaugeslights",
	"consoleclockdigitcolon",
	"tcdswitchlighton",
	"tcdswitchlightoff",
	"tcdkeypadlightson",
	"tcdkeypadenterlighton",
	"gloveboxgaugeslights",
	"dtam",
	"ptam",
	"ltdam",
	"dtpm",
	"ptpm",
	"ltdpm",
	"dtcolon",
	"ptcolon",
	"ltdcolon",
};
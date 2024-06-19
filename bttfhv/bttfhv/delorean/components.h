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
	"gloveboxgaugesglass",
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
	"roof_fr",
	"door_lf_hi_ok_fr",
	"door_lf_hi_ok_window_fr",
	"door_rf_hi_ok_fr",
	"door_rf_hi_ok_window_fr",
	"wing_lf_hi_ok_fr",
	"wing_lr_hi_ok_fr",
	"wing_rf_hi_ok_fr",
	"wing_rr_hi_ok_fr",
	"windscreen_hi_ok_fr",
	"chassis_hi_fr",
	"vents_fr",
	"bonnet_hi_ok_fr",
	"fxwheelbttf2rbon",
	"fxwheelbttf2rfon",
	"fxwheelbttf2lbon",
	"fxwheelbttf2lfon",
	"reactorshield",
	"ventsweather",
	"frontsusp",
	"plate_back",
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
	"shockrb",
	"strutshockrb",
	"hoverjointrb",

	"strutlb",
	"holderlb",
	"shockpistonlb",
	"shocklb",
	"strutshocklb",
	"hoverjointlb",

	"strutrf",
	"holderrf",
	"shockpistonrf",
	"shockrf",
	"strutshockrf",
	"hoverjointrf",

	"strutlf",
	"holderlf",
	"shockpistonlf",
	"shocklf",
	"strutshocklf",
	"hoverjointlf",
};

const vector<string> WHEEL_STOCK_COMPONENTS = {
	"fxwheelbttf1rb",
	"fxtirebttf1rb",

	"fxwheelbttf1lb",
	"fxtirebttf1lb",

	"fxwheelbttf1rf",
	"fxtirebttf1rf",

	"fxwheelbttf1lf",
	"fxtirebttf1lf",
};

const vector<string> WHEEL_WHITEWALLS_COMPONENTS = {
	"fxwheelbttf3rb",
	"fxtirebttf3rb",
	"fxhubcapbttf3rb",

	"fxwheelbttf3lb",
	"fxtirebttf3lb",
	"fxhubcapbttf3lb",

	"fxwheelbttf3rf",
	"fxtirebttf3rf",
	"fxhubcapbttf3rf",

	"fxwheelbttf3lf",
	"fxtirebttf3lf",
	"fxhubcapbttf3lf",
};

const vector<string> WHEEL_RAILROAD_COMPONENTS = {
	"fxwheelbttf3rrrb",
	"fxwheelbttf3rrlb",
	"fxwheelbttf3rrrf",
	"fxwheelbttf3rrlf",
};

const vector<string> THRUSTER_COMPONENTS = {
	"fxthrusterbttf2rb",
	"fxthrusterbttf2lb",
	"fxthrusterbttf2rf",
	"fxthrusterbttf2lf",
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

const vector<string> FROSTED_COMPONENTS = {
	"roof_fr",
	"door_lf_hi_ok_fr",
	"door_lf_hi_ok_window_fr",
	"door_rf_hi_ok_fr",
	"door_rf_hi_ok_window_fr",
	"wing_lr_hi_ok_fr",
	"wing_rr_hi_ok_fr",
	"wing_lf_hi_ok_fr",
	"wing_rf_hi_ok_fr",
	"windscreen_hi_ok_fr",
	"chassis_hi_fr",
	"vents_fr",
	"bonnet_hi_ok_fr",
};

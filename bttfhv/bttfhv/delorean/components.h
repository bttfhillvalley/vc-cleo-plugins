#pragma once
#include <string>
#include <tuple>
#include <vector>

using namespace std;

const vector<string> PLATE_STOCK_COMPONENTS = {
	"platestock",
};

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
	"bump_front_hi_ok_grill",
};

const vector<string> TIME_MACHINE_COMPONENTS = {
	"bttfparts",
	"wing_lf_hi_sh",
	"wing_rf_hi_sh",
	"wing_lr_hi_sh",
	"wing_rr_hi_sh",
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
	"fxwheelbttf2rbon",
	"fxwheelbttf2rfon",
	"fxwheelbttf2lbon",
	"fxwheelbttf2lfon",
	"reactorshield",
	"ventsweather",
	"frontsusp",
	"plate_back",
	"grillhitch",
	"bump_front_hi_ok_grilltm",
};

const vector<string> PLUTONIUM_CHAMBER_COMPONENTS = {
	"bttf1",
	"grillhitchbracketbttf1",
	"nohookbttf1",
	"reactorlidbttf1",
};

const vector<string> MR_FUSION_COMPONENTS = {
	"bttf2",
	"mrfusion",
	"fusionlatch",
	"grillhitchbracketbttf2",
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
	"fluxemitteron",
	"fluxcoilsonlb1",
	"fluxcoilsonlb2",
	"fluxcoilsonlb3",
	"fluxcoilsonlb4",
	"fluxcoilsonrb1",
	"fluxcoilsonrb2",
	"fluxcoilsonrb3",
	"fluxcoilsonrb4",
	"fluxcoilsonf1",
	"fluxcoilsonf2",
	"fluxcoilsonf3",
	"fluxcoilsonf4",
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
	"door_lf_hi_fr",
	"door_lf_hi_window_fr",
	"door_rf_hi_fr",
	"door_rf_hi_window_fr",
	"wing_lr_hi_fr",
	"wing_rr_hi_fr",
	"wing_lf_hi_fr",
	"wing_rf_hi_fr",
	"windscreen_hi_fr",
	"chassis_hi_fr",
	"vents_fr",
	"bonnet_hi_fr",
};

const vector<string> BONNET_COMPONENTS = {
	"bonnet_hi_dam",
	"bonnet_hi_fr",
	"bonnet_hi_ok",
	"bonnetbttf3",
	"bonnetrstrut",
	"bonnetrstrutp",
	"bonnetlstrut",
	"bonnetlstrutp",
};

const vector<string> BOOT_COMPONENTS = {
	"boot_hi_dam",
	"boot_hi_ok",
};

const vector<string> DOOR_LF_COMPONENTS = {
	"door_lf_hi_dam",
	"door_lf_hi_fr",
	"door_lf_hi_ok",
	"door_lf_hi_ok_glass",
	"door_lf_hi_ok_lights",
	"door_lf_hi_dam_window",
	"door_lf_hi_ok_window",
	"door_lf_hi_window_fr",
	"doorlfstrut",
	"doorlfstrutp",
};

const vector<string> DOOR_RF_COMPONENTS = {
	"door_rf_hi_dam",
	"door_rf_hi_fr",
	"door_rf_hi_ok",
	"door_rf_hi_ok_glass",
	"door_rf_hi_ok_lights",
	"door_rf_hi_dam_window",
	"door_rf_hi_ok_window",
	"door_rf_hi_window_fr",
	"doorrfstrut",
	"doorrfstrutp",
};

const vector<string> BUMP_FRONT_COMPONENTS = {
	"bump_front_hi_dam",
	"bump_front_hi_ok",
	"bump_front_hi_ok_grill",
	"bump_front_hi_ok_grilltm",
};

const vector<string> BUMP_REAR_COMPONENTS = {
	"bump_rear_hi_dam",
	"bump_rear_hi_ok",
};

const vector<string> WINDSCREEN_COMPONENTS = {
	"windscreen_hi_dam",
	"windscreen_hi_fr",
	"windscreen_hi_ok",
	"windscreen_rearview",
};

const vector<string> WING_LF_COMPONENTS = {
	"wing_lf_hi_dam",
	"wing_lf_hi_fr",
	"wing_lf_hi_ok",
	"wing_lf_hi_sh",
};

const vector<string> WING_LR_COMPONENTS = {
	"wing_lr_hi_dam",
	"wing_lr_hi_fr",
	"wing_lr_hi_ok",
	"wing_lr_hi_ok_glass",
	"wing_lr_hi_sh",
};

const vector<string> WING_RF_COMPONENTS = {
	"wing_rf_hi_dam",
	"wing_rf_hi_fr",
	"wing_rf_hi_ok",
	"wing_rf_hi_sh",
};

const vector<string> WING_RR_COMPONENTS = {
	"wing_rr_hi_dam",
	"wing_rr_hi_fr",
	"wing_rr_hi_ok",
	"wing_rr_hi_ok_glass",
	"wing_rr_hi_sh",
};
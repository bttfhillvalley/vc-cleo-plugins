#pragma once
#include <map>

#include "components.h"
#include "enums.h"

using namespace std;

const map<int, vector<string>> PLATE_OPTIONS = {
	{ PLATE_STOCK, {} },
	{ PLATE_OUTATIME, PLATE_OUTATIME_COMPONENTS },
	{ PLATE_BARCODE, PLATE_BARCODE_COMPONENTS },
};

const map<int, vector<string>> HOOK_OPTIONS = {
	{ HOOK_NONE, {} },
	{ HOOK_HOLDER, HOOK_HOLDER_COMPONENTS },
	{ HOOK_SIDE, HOOK_SIDE_COMPONENTS },
	{ HOOK_UP, HOOK_UP_COMPONENTS },
};

const map<int, vector<string>> HOOD_OPTIONS = {
	{ HOOD_STOCK, {} },
	{ HOOD_HOODBOX, HOODBOX_COMPONENTS },
};

const map<int, vector<string>> HITCH_OPTIONS = {
	{ HITCH_NONE, {} },
	{ HITCH_ATTACHED, HITCH_COMPONENTS },
};

const map<int, vector<string>> BODY_OPTIONS = {
	{ BODY_STOCK, STOCK_COMPONENTS },
	{ BODY_TIME_MACHINE, TIME_MACHINE_COMPONENTS },
};

const map<int, vector<string>> REAR_DECK_OPTIONS = {
	{ REAR_DECK_PLUTONIUM, PLUTONIUM_CHAMBER_COMPONENTS },
	{ REAR_DECK_FUSION, MR_FUSION_COMPONENTS },
};

const map<int, vector<string>> GRILL_HITCH_OPTIONS = {
	{ GRILL_HITCH_STOCK, GRILL_HITCH_STOCK_COMPONENTS },
	{ GRILL_HITCH_TM, GRILL_HITCH_TM_COMPONENTS },
};

const map<int, vector<string>> GRILL_BRACKET_OPTIONS = {
	{ GRILL_BRACKET_STOCK, GRILL_BRACKET_STOCK_COMPONENTS },
	{ GRILL_BRACKET_HOVER, GRILL_BRACKET_HOVER_COMPONENTS },
};

const map<int, vector<string>> DRIVETRAIN_OPTIONS{
	{ DRIVETRAIN_STOCK, DRIVETRAIN_STOCK_COMPONENTS },
	{ DRIVETRAIN_HOVER, DRIVETRAIN_HOVER_COMPONENTS },
};

const map<int, vector<string>> WHEEL_OPTIONS = {
	{ WHEEL_STOCK, WHEEL_STOCK_COMPONENTS },
	{ WHEEL_WHITEWALLS, WHEEL_WHITEWALLS_COMPONENTS },
	{ WHEEL_RAILROAD, WHEEL_RAILROAD_COMPONENTS },
};

const map<int, vector<string>> BULOVA_CLOCK_OPTIONS = {
	{ BULOVA_CLOCK_NONE, {} },
	{ BULOVA_CLOCK_ATTACHED, BULOVA_CLOCK_COMPONENTS },
};

const map<int, vector<string>> FIREBOX_GAUGE_OPTIONS = {
	{ FIREBOX_GAUGE_NONE, {} },
	{ FIREBOX_GAUGE_ATTACHED, FIREBOX_GAUGE_COMPONENTS },
};

const map<int, string> SHIFTER_VALUES = {
	{ SHIFTER_N, "shiftern" },
	{ SHIFTER_R, "shifterr" },
	{ SHIFTER_1, "shifter1" },
	{ SHIFTER_2, "shifter2" },
	{ SHIFTER_3, "shifter3" },
	{ SHIFTER_4, "shifter4" },
	{ SHIFTER_5, "shifter5" },
};
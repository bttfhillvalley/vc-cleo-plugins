#pragma once

enum Plate {
	PLATE_NONE,
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
	BODY_STOCK,
	BODY_TIME_MACHINE,
};

enum RearDeck {
	REAR_DECK_NONE,
	REAR_DECK_PLUTONIUM,
	REAR_DECK_FUSION,
};

enum BulovaClock {
	BULOVA_CLOCK_NONE,
	BULOVA_CLOCK_ATTACHED,
};

enum FireboxGauge {
	FIREBOX_GAUGE_NONE,
	FIREBOX_GAUGE_ATTACHED,
};

enum Shifter {
	SHIFTER_N = -1,
	SHIFTER_R,
	SHIFTER_1,
	SHIFTER_2,
	SHIFTER_3,
	SHIFTER_4,
	SHIFTER_5,
};

enum BONNET_STATE {
	BONNET_CLOSED,
	BONNET_CLOSING,
	BONNET_OPENING,
	BONNET_OPEN
};

enum BOOT_STATE {
	BOOT_CLOSED,
	BOOT_CLOSING,
	BOOT_OPENING,
	BOOT_OPEN
};

#include "../constants.h"
#include "../vehicle/components.h"
#include "delorean.h"
#include <iostream>
#define HOVER_WHEELS 0xAA

const map<int, tuple<float, float>> GEAR_RATIOS = {
	{ SHIFTER_N, { 7.602f, 18.285f }},
	{ SHIFTER_R, { 7.602f, 10.0f }},
	{ SHIFTER_1, { 7.602f, 18.285f }},
	{ SHIFTER_2, { 4.838f, 12.636f }},
	{ SHIFTER_3, { 3.326f, 10.0f }},
	{ SHIFTER_4, { 2.534f, 10.0f }},
	{ SHIFTER_5, { 1.971f, 8.926f }},
};

void Delorean::SetupShifter() {
	for (const auto& it : SHIFTER_VALUES) {
		setVisibility(automobile, it.second, 0);
	}
}

void Delorean::ProcessShifter() {
	int wheelStatus = getWheelStatusAll(automobile);
	int gear = -1;
	float target;
	if (GetSpeed() > 1.0f && wheelStatus != HOVER_WHEELS)
	{
		gear = automobile->m_nCurrentGear;
	}
	string component = SHIFTER_VALUES.at(gear);
	if (!getVisibility(automobile, component)) {
		SetupShifter();
		setVisibility(automobile, component, 1);
	}

	// Animate RPM Needle
	if (wheelStatus != HOVER_WHEELS) {
		auto ratio = GEAR_RATIOS.at(gear);
		auto wheelSpeed = max(automobile->fWheelSpeed[CARWHEEL_REAR_RIGHT], automobile->fWheelSpeed[CARWHEEL_REAR_RIGHT]);
		target = clamp(11.85f * abs(wheelSpeed) * get<0>(ratio) + get<1>(ratio), 5.0f, 360.0f);
	}
	else {
		auto ratio = GEAR_RATIOS.at(SHIFTER_5);
		target = GetSpeed();
		target = clamp(GetSpeed() * get<0>(ratio) + get<1>(ratio), 18.285f, 360.0f);
	}
	float delta = clamp(target - rpmNeedle, -5.0f, 5.0f);
	rpmNeedle += delta;
	rotateComponent(automobile, "rpmneedle", 0.0f, rpmNeedle, 0.0f);
}
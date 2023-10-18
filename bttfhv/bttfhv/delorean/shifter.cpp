#include "../vehicle/components.h"
#include "delorean.h"
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
		setVisibility(timeMachine, it.second, 0);
	}
}

void Delorean::AnimateShifter() {
	int wheelStatus = getWheelStatusAll(timeMachine);
	int gear = -1;
	if (GetSpeed() > 1.0f && wheelStatus != HOVER_WHEELS)
	{
		gear = timeMachine->m_nCurrentGear;
	}
	string component = SHIFTER_VALUES.at(gear);
	if (!getVisibility(timeMachine, component)) {
		SetupShifter();
		setVisibility(timeMachine, component, 1);
	}

	// Animate RPM Needle
	auto ratio = GEAR_RATIOS.at(gear);
	float needle = GetSpeed() * get<0>(ratio) + get<1>(ratio);
	CVector rotation = getComponentRotation(timeMachine, "rpmneedle");
	float delta = needle - rotation.y;
	rotation.y += clamp(delta, -5.0f, 5.0f);
	rotateComponent(timeMachine, "rpmneedle", rotation);
}
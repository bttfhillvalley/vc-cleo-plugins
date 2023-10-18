#include "../vehicle/components.h"
#include "components.h"
#include "delorean.h"

void Delorean::SetupTimeCircuits() {
	int index;
	string digit;

	// Months
	for (index = 1; index <= 12; index++) {

		SetGlowAndHideIndex(timeMachine, "dtmonth", index);
		SetGlowAndHideIndex(timeMachine, "ptmonth", index);
		SetGlowAndHideIndex(timeMachine, "ltdmonth", index);
	}

	// Days
	for (index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(timeMachine, "dtday", index);
		SetGlowAndHideIndex(timeMachine, "ptday", index);
		SetGlowAndHideIndex(timeMachine, "ltdday", index);
	}

	// Years
	for (index = 10; index <= 49; index++) {
		SetGlowAndHideIndex(timeMachine, "dtyear", index);
		SetGlowAndHideIndex(timeMachine, "ptyear", index);
		SetGlowAndHideIndex(timeMachine, "ltdyear", index);
	}

	// Hours
	for (index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(timeMachine, "dthour", index);
		SetGlowAndHideIndex(timeMachine, "pthour", index);
		SetGlowAndHideIndex(timeMachine, "ltdhour", index);
		SetGlowAndHideIndex(timeMachine, "consoleclockdigithour", index);
	}

	// Minutes
	for (index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(timeMachine, "dtmin", index);
		SetGlowAndHideIndex(timeMachine, "ptmin", index);
		SetGlowAndHideIndex(timeMachine, "ltdmin", index);
		SetGlowAndHideIndex(timeMachine, "consoleclockdigitmin", index);
	}
}
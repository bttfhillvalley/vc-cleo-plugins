#include "../vehicle/components.h"
#include "components.h"
#include "delorean.h"

void Delorean::SetupTimeCircuits() {
	int index;
	string digit;

	// Months
	for (index = 1; index <= 12; index++) {

		SetGlowAndHideIndex(automobile, "dtmonth", index);
		SetGlowAndHideIndex(automobile, "ptmonth", index);
		SetGlowAndHideIndex(automobile, "ltdmonth", index);
	}

	// Days
	for (index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(automobile, "dtday", index);
		SetGlowAndHideIndex(automobile, "ptday", index);
		SetGlowAndHideIndex(automobile, "ltdday", index);
	}

	// Years
	for (index = 10; index <= 49; index++) {
		SetGlowAndHideIndex(automobile, "dtyear", index);
		SetGlowAndHideIndex(automobile, "ptyear", index);
		SetGlowAndHideIndex(automobile, "ltdyear", index);
	}

	// Hours
	for (index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(automobile, "dthour", index);
		SetGlowAndHideIndex(automobile, "pthour", index);
		SetGlowAndHideIndex(automobile, "ltdhour", index);
		SetGlowAndHideIndex(automobile, "consoleclockdigithour", index);
	}

	// Minutes
	for (index = 10; index <= 29; index++) {
		SetGlowAndHideIndex(automobile, "dtmin", index);
		SetGlowAndHideIndex(automobile, "ptmin", index);
		SetGlowAndHideIndex(automobile, "ltdmin", index);
		SetGlowAndHideIndex(automobile, "consoleclockdigitmin", index);
	}
}
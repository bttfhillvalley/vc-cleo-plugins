#include "../vehicle/components.h"
#include "delorean.h"

void Delorean::SetupSid() {
	string led;
	for (int column = 1; column <= 10; column++) {
		for (int row = 0; row <= 20; row++) {
			led = getComponentIndex("sidledsline", (column == 10) ? row * 100 + column : row * 10 + column);
			setGlow(timeMachine, led, 1);
			setVisibility(timeMachine, led, 0);
		}
	}
}
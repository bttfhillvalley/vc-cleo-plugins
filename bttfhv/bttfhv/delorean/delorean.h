#pragma once
#include <map>

#include "plugin.h"

using namespace std;

class Delorean
{
public:
	int vehicleHandle;

	CAutomobile* timeMachine;

	Delorean(int handle);


	void AnimateDoorStruts();
};

extern map<int, Delorean> deloreanMap;
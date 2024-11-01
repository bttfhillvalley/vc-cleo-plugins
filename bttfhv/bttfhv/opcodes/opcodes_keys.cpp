#include "opcodes.h"


eOpcodeResult __stdcall isAnaloguePressed(CScript* script)
{
	script->Collect(2);
	CPad* pad = CPad::GetPad(Params[0].nVar);
	int state = Params[1].nVar;
	script->UpdateCompareFlag(
		(state == 1 && (pad->GetAnaloguePadUp() || pad->NewState.DPadUp)) ||
		(state == 2 && (pad->GetAnaloguePadDown() || pad->NewState.DPadDown)) ||
		(state == 3 && (pad->GetAnaloguePadLeft() || pad->NewState.DPadLeft)) ||
		(state == 4 && (pad->GetAnaloguePadRight() || pad->NewState.DPadRight)) ||
		(state == 5 && pad->NewState.ButtonCross) ||
		(state == 6 && pad->NewState.ButtonCircle)
	);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isAnalogueNotPressed(CScript* script)
{
	script->Collect(2);
	CPad* pad = CPad::GetPad(Params[0].nVar);
	int state = Params[1].nVar;
	script->UpdateCompareFlag(!(
		(state == 1 && (pad->GetAnaloguePadUp() || pad->NewState.DPadUp)) ||
		(state == 2 && (pad->GetAnaloguePadDown() || pad->NewState.DPadDown)) ||
		(state == 3 && (pad->GetAnaloguePadLeft() || pad->NewState.DPadLeft)) ||
		(state == 4 && (pad->GetAnaloguePadRight() || pad->NewState.DPadRight)) ||
		(state == 5 && pad->NewState.ButtonCross) ||
		(state == 6 && pad->NewState.ButtonCircle)
	));
	return OR_CONTINUE;
}
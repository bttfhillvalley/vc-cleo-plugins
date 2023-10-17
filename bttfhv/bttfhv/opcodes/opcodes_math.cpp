#include <cmath>

#include "../utils/math.h"
#include "opcodes.h"

eOpcodeResult __stdcall getArcsin(CScript* script) {
	script->Collect(1);
	Params[0].fVar = degrees(asinf(Params[0].fVar));
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall getArccos(CScript* script) {
	script->Collect(1);
	Params[0].fVar = degrees(acosf(Params[0].fVar));
	script->Store(1);
	return OR_CONTINUE;
}


eOpcodeResult __stdcall getTan(CScript* script) {
	script->Collect(1);
	Params[0].fVar = tanf(radians(Params[0].fVar));
	script->Store(1);
	return OR_CONTINUE;
}


eOpcodeResult __stdcall getArctan(CScript* script) {
	script->Collect(1);
	Params[0].fVar = degrees(atanf(Params[0].fVar));
	script->Store(1);
	return OR_CONTINUE;
}
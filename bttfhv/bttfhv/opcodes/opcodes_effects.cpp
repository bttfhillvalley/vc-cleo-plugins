#include "CPointLights.h"
#include "opcodes.h"

eOpcodeResult __stdcall createLight(CScript* script)
{
	script->Collect(12);
	CVector point = CVector(Params[1].fVar, Params[2].fVar, Params[3].fVar);
	CVector direction = CVector(Params[4].fVar, Params[5].fVar, Params[6].fVar);
	CPointLights::AddLight(Params[0].nVar, point, direction, Params[7].fVar, Params[8].fVar, Params[9].fVar, Params[10].fVar, Params[11].nVar, false);
	return OR_CONTINUE;
}
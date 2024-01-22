#include "CPointLights.h"
#include "CParticle.h"
#include "opcodes.h"

eOpcodeResult __stdcall createLight(CScript* script)
{
	script->Collect(12);
	CVector point = CVector(Params[1].fVar, Params[2].fVar, Params[3].fVar);
	CVector direction = CVector(Params[4].fVar, Params[5].fVar, Params[6].fVar);
	CPointLights::AddLight(Params[0].nVar, point, direction, Params[7].fVar, Params[8].fVar, Params[9].fVar, Params[10].fVar, Params[11].nVar, false);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall createParticle(CScript* script)
{
	script->Collect(12);
	CVector pos = CVector(Params[1].fVar, Params[2].fVar, Params[3].fVar);
	CVector vel = CVector(Params[4].fVar, Params[5].fVar, Params[6].fVar);
	RwRGBA color;

	color.red = Params[8].nVar;
	color.green = Params[9].nVar;
	color.blue = Params[10].nVar;
	color.alpha = Params[11].nVar;
	CParticle::AddParticle((tParticleType)Params[0].nVar, pos, vel, NULL, Params[7].fVar, &color, 0, 0, 0, 0);
	return OR_CONTINUE;
}
#include "CPointLights.h"
#include "CParticle.h"
#include "CPad.h"
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
	script->Collect(13);
	CVector pos = CVector(Params[1].fVar, Params[2].fVar, Params[3].fVar);
	CVector vel = CVector(Params[4].fVar, Params[5].fVar, Params[6].fVar);
	RwRGBA color;

	color.red = Params[8].nVar;
	color.green = Params[9].nVar;
	color.blue = Params[10].nVar;
	color.alpha = Params[11].nVar;
	CParticle::AddParticle((tParticleType)Params[0].nVar, pos, vel, NULL, Params[7].fVar, &color, 0, 0, 0, Params[12].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall shakePad(CScript* script)
{
	script->Collect(3);
	CPad::GetPad(Params[0].nVar)->StartShake(Params[1].nVar, Params[2].nVar);
	return OR_CONTINUE;
}

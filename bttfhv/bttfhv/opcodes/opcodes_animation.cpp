#include "opcodes.h"

eOpcodeResult __stdcall playCharAnim(CScript* script)
{
	script->Collect(4);
	CPed* animped = CPools::GetPed(Params[3].nVar);
	plugin::Call<0x405640>(animped->m_pRwClump, Params[1].nVar, Params[0].nVar, Params[2].fVar);
	return OR_CONTINUE;
}
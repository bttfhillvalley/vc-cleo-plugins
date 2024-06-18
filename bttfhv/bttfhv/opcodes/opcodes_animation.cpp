#include "opcodes.h"
#include "AnimationId.h"


eOpcodeResult __stdcall playCharAnim(CScript* script)
{
	script->Collect(4);
	CPed* animped = CPools::GetPed(Params[3].nVar);
	plugin::Call<0x405640>(animped->m_pRwClump, Params[1].nVar, Params[0].nVar, Params[2].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall createKnockback(CScript* script)
{
	script->Collect(5);
	CVector pos = CVector(Params[0].fVar, Params[1].fVar, Params[2].fVar);
	CVector2D area = CVector2D(Params[0].fVar, Params[1].fVar);
	float fRadius = Params[3].fVar;
	float fPower = Params[4].fVar;
	plugin::Call<0x489FA0>(EVENT_EXPLOSION, pos, 1000);
	for (CPed* ped: CPools::ms_pPedPool) {
		CVector vecDistance = ped->m_placement.pos - pos;
		float fMagnitude = vecDistance.Magnitude();
		if (ped->m_bInVehicle || !ped->m_nPedFlags.bIsStanding) {
			continue;
		}
		ped->m_nPedFlags.bIsStanding = false;
		ped->CheckForExplosions(area);
		ped->SetFlee(area, 10000);

		if (fMagnitude > fRadius) {
			continue;
		}

		float fDamageMultiplier = min((fRadius - fMagnitude) * 2.0f / fRadius, 1.0f);
		CVector vecForceDir =
			vecDistance * (fPower * ped->m_fMass / 1400.0f * fDamageMultiplier /
				max(fMagnitude, 0.01f));
		vecForceDir.z = max(vecForceDir.z, 0.0f);
		if (ped == FindPlayerPed()) vecForceDir.z = min(vecForceDir.z, 1.0f);
		ped->ApplyMoveForce(vecForceDir);
		int direction = ped->GetLocalDirection(CVector2D(-vecForceDir.x, -vecForceDir.y));
		ped->ApplyMoveForce(0.0, 0.0, 2.0f);
		plugin::CallMethod<0x4FD9F0, CPed*, int, AnimationId, unsigned char>(ped, 2000, (AnimationId)(direction + ANIM_STD_HIGHIMPACT_FRONT), 0);

	}

	return OR_CONTINUE;
}
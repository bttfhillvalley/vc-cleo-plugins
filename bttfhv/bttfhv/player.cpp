#include "CWorld.h"

#include "player.h"

bool isPlayerInSphere(CVector p, CVector d) {
	CPlayerInfo* playerInfo = &CWorld::Players[CWorld::PlayerInFocus];
	CVector pos = playerInfo->m_pPed->GetPosition();
	return p.x - d.x <= pos.x &&
		p.x + d.x >= pos.x &&
		p.y - d.y <= pos.y &&
		p.y + d.y >= pos.y &&
		p.z - d.z <= pos.z &&
		p.z + d.z >= pos.z;
}
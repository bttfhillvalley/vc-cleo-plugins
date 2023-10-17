#include "CTxdStore.h"
#include "opcodes.h"

eOpcodeResult __stdcall replaceTex(CScript* script)
{
	CTxdStore::PushCurrentTxd();
	script->Collect(5);
	RwTexture* replacement, * original;
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot(Params[1].cVar));
	char texture[256];
	sprintf(texture, "%s%d", Params[3].cVar, Params[4].nVar);
	replacement = RwTextureRead(texture, NULL);
	if (replacement) {
		original = RwTextureRead(Params[2].cVar, NULL);
		if (original) {
			memcpy(original, replacement, sizeof(replacement));
		}
		RwTextureDestroy(replacement);
	}
	CTxdStore::PopCurrentTxd();
	return OR_CONTINUE;
}

eOpcodeResult __stdcall loadTxdDict(CScript* script)
{
	script->Collect(1);
	char fullpath[128];
	snprintf(fullpath, 128, "models\\%s.txd", Params[0].cVar);
	int slot = CTxdStore::FindTxdSlot("script");
	if (slot == -1)
		slot = CTxdStore::AddTxdSlot("script");
	CTxdStore::LoadTxd(slot, fullpath);
	CTxdStore::AddRef(slot);
	return OR_CONTINUE;
}
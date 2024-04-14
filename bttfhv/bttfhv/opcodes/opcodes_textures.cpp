#include "CTxdStore.h"
#include "opcodes.h"

void _replaceTex(const char* originalTxd, const char* originalTexture, const char* replacementTxd, const char* replacementTexture) {
	CTxdStore::PushCurrentTxd();
	RwTexture *replacement, *original, *temp;
	CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot(replacementTxd));
	replacement = RwTextureRead(replacementTexture, NULL);
	if (replacement) {
		temp = new RwTexture();
		memcpy(temp, replacement, sizeof(replacement));
		CTxdStore::PopCurrentTxd();
		CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot(originalTxd));
		original = RwTextureRead(originalTexture, NULL);
		if (original) {
			memcpy(original, replacement, sizeof(replacement));
		}
		RwTextureDestroy(replacement);
	}
	CTxdStore::PopCurrentTxd();
}

eOpcodeResult __stdcall replaceTex(CScript* script)
{
	script->Collect(4);
	_replaceTex(Params[0].cVar, Params[1].cVar, Params[2].cVar, Params[3].cVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall replaceTexIndex(CScript* script)
{
	script->Collect(5);
	char texture[256];
	sprintf(texture, "%s%d", Params[3].cVar, Params[4].nVar);
	_replaceTex(Params[0].cVar, Params[1].cVar, Params[2].cVar, texture);
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
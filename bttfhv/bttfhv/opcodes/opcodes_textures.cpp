#include "CTxdStore.h"
#include "opcodes.h"

void _loadTxd(const char* txd) {
	int slot = CTxdStore::FindTxdSlot(txd);
	if (slot == -1) {
		slot = CTxdStore::AddTxdSlot(txd);
		char txd[256];
		sprintf(txd, "%s", txd);
		CTxdStore::LoadTxd(slot, txd);
		CTxdStore::AddRef(slot);
	}
	CTxdStore::SetCurrentTxd(slot);
}

void _replaceTex(const char* originalTxd, const char* originalTexture, const char* replacementTxd, const char* replacementTexture) {
	RwTexture *replacement, *original, *temp;
	_loadTxd(replacementTxd);
	replacement = RwTextureRead(replacementTexture, NULL);
	if (replacement) {
		CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot(originalTxd));
		original = RwTextureRead(originalTexture, NULL);
		if (original) {
			memcpy(original, replacement, sizeof(replacement));
		}
		RwTextureDestroy(replacement);
	}
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

void _drawSprite(const char* txd, const char* texture, CRect source, CRect dest, float angle)
{
	RwTexture* rwTexture;
	_loadTxd(txd);
	rwTexture = RwTextureRead(texture, NULL);
	if (rwTexture) {

	}
}

eOpcodeResult __stdcall drawSprite(CScript* script)
{
	// txddict texture source(4), dest(4) angle
	script->Collect(11);
	CRect source(Params[2].fVar, Params[3].fVar, Params[4].fVar, Params[5].fVar);
	CRect dest(Params[6].fVar, Params[7].fVar, Params[8].fVar, Params[9].fVar);
	float angle = Params[10].fVar;
	_drawSprite(Params[0].cVar, Params[1].cVar, source, dest, angle);
	return OR_CONTINUE;
}
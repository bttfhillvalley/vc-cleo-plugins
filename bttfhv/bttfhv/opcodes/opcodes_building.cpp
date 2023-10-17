#include <set>
#include "CWorld.h"
#include "../building/idemap.h"

#include "opcodes.h"

eOpcodeResult __stdcall addBuilding(CScript* script)
{
	script->Collect(1);
	set<int>* models;
	models = getModels(Params[0].cVar);
	for (auto object : CPools::ms_pBuildingPool) {
		if (models->find(object->m_nModelIndex) != models->end()) {
			CWorld::Add(object);
		}
	}
	/*for (auto object : CPools::ms_pDummyPool) {
		if (models.find(object->m_nModelIndex) != models.end()) {
			CWorld::Add(object);
		}
	}*/
	return OR_CONTINUE;
}

eOpcodeResult __stdcall removeBuilding(CScript* script)
{
	script->Collect(1);
	set<int>* models;
	models = getModels(Params[0].cVar);
	for (auto object : CPools::ms_pBuildingPool) {
		if (models->find(object->m_nModelIndex) != models->end()) {
			//of << object->m_nModelIndex << "," << (int)object->m_nType << std::endl;
			CWorld::Remove(object);
		}
	}

	return OR_CONTINUE;
}

eOpcodeResult __stdcall addObjects(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	if (removeObjectQueue.contains(key)) {
		removeObjectQueue.erase(key);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall removeObjects(CScript* script)
{
	script->Collect(1);
	set<int>* models;
	models = getModels(Params[0].cVar);
	string key(Params[0].cVar);
	if (!removeObjectQueue.contains(key)) {
		removeObjectQueue[key] = models;
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setBuildingComponentVisibility(CScript* script)
{
	script->Collect(3);
	for (auto object : CPools::ms_pBuildingPool) {
		if (object->m_nModelIndex == Params[0].nVar) {
			//setVisibility(object, Params[1].cVar, Params[2].nVar);
			break;
		}
	}
	return OR_CONTINUE;
}
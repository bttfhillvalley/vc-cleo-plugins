#pragma once
#include "plugin.h"

int GetAtomicId(RpAtomic* atomic);
RpMaterial* SetAmbientCB(RpMaterial* material, void* data);
RpMaterial* SetRedCB(RpMaterial* material, void* data);
RpMaterial* SetGreenCB(RpMaterial* material, void* data);
RpMaterial* SetBlueCB(RpMaterial* material, void* data);
RwObject* SetAtomicVisibilityCB(RwObject* rwObject, void* data);
RwObject* GetAtomicVisibilityCB(RwObject* rwObject, void* data);
RwObject* GetAtomicObjectCB(RwObject* object, void* data);
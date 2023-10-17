#include "utils.h"

int& ms_atomicPluginOffset = *(int*)0x69A1C8;

int GetAtomicId(RpAtomic* atomic) {
	return *(&atomic->object.object.type + ms_atomicPluginOffset);
}

RpMaterial* SetAmbientCB(RpMaterial* material, void* data)
{
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	if (data == (void*)(0))
		properties->ambient = 0.5f;
	else
		properties->ambient = 5.0f;
	return material;
}

RpMaterial* SetRedCB(RpMaterial* material, void* data)
{
	unsigned int value = reinterpret_cast<unsigned int>(data);
	RwRGBA* col = (RwRGBA*)RpMaterialGetColor(material);	// get rid of const
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	col->red = value;
	return material;
}

RpMaterial* SetGreenCB(RpMaterial* material, void* data)
{
	unsigned int value = reinterpret_cast<unsigned int>(data);
	RwRGBA* col = (RwRGBA*)RpMaterialGetColor(material);	// get rid of const
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	col->green = value;
	return material;
}

RpMaterial* SetBlueCB(RpMaterial* material, void* data)
{
	unsigned int value = reinterpret_cast<unsigned int>(data);
	RwRGBA* col = (RwRGBA*)RpMaterialGetColor(material);	// get rid of const
	RwSurfaceProperties* properties = (RwSurfaceProperties*)RpMaterialGetSurfaceProperties(material);
	col->blue = value;
	return material;
}

RwObject* __cdecl SetAtomicVisibilityCB(RwObject* rwObject, void* data) {
	if (data == (void*)(0))
		rwObject->flags = 0;
	else
		rwObject->flags = 4;
	return rwObject;
}

RwObject* __cdecl GetAtomicVisibilityCB(RwObject* rwObject, void* data) {
	int* visibility = static_cast<int*>(data);
	*visibility = (int)rwObject->flags;
	return rwObject;
}

RwObject* __cdecl GetAtomicObjectCB(RwObject* object, void* data)
{
	*(RpAtomic**)data = (RpAtomic*)object;
	return object;
}
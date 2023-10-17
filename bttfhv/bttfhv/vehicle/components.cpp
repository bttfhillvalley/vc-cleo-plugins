#include "CClumpModelInfo.h"

#include "../rw/utils.h"
#include "../utils/math.h"
#include "components.h"


void setVisibility(CEntity* model, const char* component, int visible) {
	int visibility = 0;
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
		if (frame) {
			RwFrameForAllObjects(frame, GetAtomicVisibilityCB, &visibility);
			if (visible != visibility) {
				RwFrameForAllObjects(frame, SetAtomicVisibilityCB, (void*)visible);
			}
		}
	}
}

int getVisibility(CEntity* model, const char* component) {
	int visibility = 0;
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
		if (frame) {
			RwFrameForAllObjects(frame, GetAtomicVisibilityCB, &visibility);
		}
	}
	return visibility;
}

void moveComponent(CEntity* model, const char* component, float x, float y, float z) {
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
		if (frame) {
			CMatrix cmmatrix(&frame->modelling, false);
			cmmatrix.SetTranslateOnly(x, y, z);
			cmmatrix.UpdateRW();
		}
	}
}

void rotateComponent(CEntity* model, const char* component, float rx, float ry, float rz) {
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component);
		if (frame) {
			CMatrix cmatrix(&frame->modelling, false);
			CVector cpos(cmatrix.pos);
			cmatrix.SetRotate(radians(rx), radians(ry), radians(rz));
			cmatrix.pos = cpos;
			cmatrix.UpdateRW();
		}
	}
}

void setColor(CVehicle* vehicle, const char* component, int red, int green, int blue) {
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
		if (frame) {
			RpAtomic* atomic;
			RpGeometry* geometry;
			RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
			geometry = atomic->geometry;
			RpGeometryForAllMaterials(geometry, SetRedCB, (void*)red);
			RpGeometryForAllMaterials(geometry, SetGreenCB, (void*)green);
			RpGeometryForAllMaterials(geometry, SetBlueCB, (void*)blue);
		}
	}
}

void setAlpha(CVehicle* vehicle, const char* component, int alpha) {
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
		if (frame) {
			RpAtomic* atomic;
			RpGeometry* geometry;
			RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
			geometry = atomic->geometry;
			vehicle->SetComponentAtomicAlpha(atomic, alpha);
			RwFrameForAllObjects(frame, SetAtomicVisibilityCB, (void*)alpha);
		}
	}
}

RwUInt8 getAlpha(CVehicle* vehicle, const char* component) {
	RwUInt8 alpha = 0;
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);

		if (frame) {
			RpAtomic* atomic;
			RpGeometry* geometry;
			RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
			geometry = atomic->geometry;
			alpha = atomic->geometry->matList.materials[0]->color.alpha;
		}

	}
	return alpha;
}

void fadeAlpha(CVehicle* vehicle, const char* component, int target, int fade) {
	int alpha = getAlpha(vehicle, component);
	target = max(0, target);
	target = min(target, 255);
	if (alpha > target) {
		alpha -= fade;
		alpha = max(alpha, target);
	}
	else if (alpha < target) {
		alpha += fade;
		alpha = min(alpha, target);
	}
	setAlpha(vehicle, component, alpha);
}

void setGlow(CVehicle* vehicle, const char* component, int glow) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component);
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		RpGeometryForAllMaterials(geometry, SetAmbientCB, (void*)glow);
	}
}

int getCurrentDigit(CVehicle* vehicle, const char* component) {
	char digitComponent[128];
	for (int digit = 0; digit < 20; digit++) {
		sprintf(digitComponent, "%s%d", component, digit);
		int visibility = getVisibility(vehicle, digitComponent);
		if (visibility > 0) {
			return digit;
		}
	}
	return -1;
}

void digitOff(CVehicle* vehicle, const char* component) {
	char digitComponent[128];
	int digit = getCurrentDigit(vehicle, component);
	if (digit != -1) {
		sprintf(digitComponent, "%s%d", component, digit);
		setVisibility(vehicle, digitComponent, 0);
	}
}

void digitOn(CVehicle* vehicle, const char* component, int digit) {
	if (digit == -1) {
		digitOff(vehicle, component);
		return;
	}
	char digitComponent[128];
	sprintf(digitComponent, "%s%d", component, digit);
	int visibility = getVisibility(vehicle, digitComponent);
	if (visibility == 0) {
		digitOff(vehicle, component);
		setVisibility(vehicle, digitComponent, 1);
	}
}
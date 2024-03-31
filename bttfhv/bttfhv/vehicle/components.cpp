#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <sstream>
#include "CClumpModelInfo.h"
#include "CModelInfo.h"
#include "CVehicleModelInfo.h"

#include "../rw/utils.h"
#include "../utils/math.h"
#include "components.h"

string getComponentIndex(string name, int index) {
	stringstream ss;
	ss << name << index;
	return ss.str();
}

void setVisibility(CEntity* model, string component, int visible) {
	int visibility = 0;
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component.c_str());
		if (frame) {
			RwFrameForAllObjects(frame, GetAtomicVisibilityCB, &visibility);
			if (visible != visibility) {
				RwFrameForAllObjects(frame, SetAtomicVisibilityCB, (void*)visible);
			}
		}
	}
}

int getVisibility(CEntity* model, string component) {
	int visibility = 0;
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component.c_str());
		if (frame) {
			RwFrameForAllObjects(frame, GetAtomicVisibilityCB, &visibility);
		}
	}
	return visibility;
}

void moveComponent(CEntity* model, string component, float x, float y, float z) {
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component.c_str());
		if (frame) {
			CMatrix cmmatrix(&frame->modelling, false);
			cmmatrix.SetTranslateOnly(x, y, z);
			cmmatrix.UpdateRW();
		}
	}
}

void rotateComponent(CEntity* model, string component, float rx, float ry, float rz) {
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component.c_str());
		if (frame) {
			CMatrix cmatrix(&frame->modelling, false);
			CVector cpos(cmatrix.pos);
			cmatrix.SetRotate(radians(rx), radians(ry), radians(rz));
			cmatrix.pos = cpos;
			cmatrix.UpdateRW();
		}
	}
}

CVector getComponentRotation(CEntity* model, string component) {
	CVector rotation;
	if (model) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(model->m_pRwClump, component.c_str());
		if (frame) {
			CMatrix cmatrix(&frame->modelling, false);
			float sy = sqrtf(cmatrix.right.x * cmatrix.right.x + cmatrix.up.x * cmatrix.up.x);
			if (sy > 1e-6) {
				rotation.x = atan2f(-cmatrix.at.y, cmatrix.at.z);
				rotation.y = atan2f(-cmatrix.at.x, sy);
				rotation.z = atan2f(cmatrix.up.x, cmatrix.right.x);
			}
			else {
				rotation.x = atan2f(-cmatrix.up.z, cmatrix.up.y);
				rotation.y = atan2f(-cmatrix.at.x, sy);
				rotation.z = 0.0f;
			}

			rotation.x = degrees(rotation.x);
			rotation.y = degrees(rotation.y);
			rotation.z = degrees(rotation.z);
		}
	}
	return rotation;
}

void setColor(CVehicle* vehicle, string component, int red, int green, int blue) {
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component.c_str());
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

void setAlpha(CVehicle* vehicle, string component, int alpha) {
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component.c_str());
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

RwUInt8 getAlpha(CVehicle* vehicle, string component) {
	RwUInt8 alpha = 0;
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component.c_str());

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

void fadeAlpha(CVehicle* vehicle, string component, int target, int fade) {
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

void setGlow(CVehicle* vehicle, string component, int glow) {
	RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component.c_str());
	if (frame) {
		RpAtomic* atomic;
		RpGeometry* geometry;
		RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
		geometry = atomic->geometry;
		RpGeometryForAllMaterials(geometry, SetAmbientCB, (void*)glow);
	}
}

int getCurrentDigit(CVehicle* vehicle, string component) {
	for (int digit = 0; digit < 71; digit++) {
		string digitComponent = getComponentIndex(component, digit);
		int visibility = getVisibility(vehicle, digitComponent);
		if (visibility > 0) {
			return digit;
		}
	}
	return -1;
}

void digitOff(CVehicle* vehicle, string component) {
	int digit = getCurrentDigit(vehicle, component);
	if (digit != -1) {
		string digitComponent = getComponentIndex(component, digit);
		setVisibility(vehicle, digitComponent, 0);
	}
}

void digitOn(CVehicle* vehicle, string component, int digit) {
	if (digit == -1) {
		digitOff(vehicle, component);
		return;
	}
	string digitComponent = getComponentIndex(component, digit);
	int visibility = getVisibility(vehicle, digitComponent);
	if (visibility == 0) {
		digitOff(vehicle, component);
		setVisibility(vehicle, digitComponent, 1);
	}
}

unsigned char getWheelStatusAll(CVehicle* vehicle) {
	unsigned char status = 0;
	CAutomobile* automobile = reinterpret_cast<CAutomobile*>(vehicle);
	for (int n = 0; n < 4; n++) {
		status <<= 2;
		status |= automobile->m_carDamage.GetWheelStatus(n);
	}
	return status;
}

void SetGlowAndHideIndex(CVehicle* vehicle, string component, int index) {
	string componentIndex;
	if (index >= 0) {
		componentIndex = getComponentIndex(component, index);
	}
	else {
		componentIndex = component;
	}
	setGlow(vehicle, componentIndex, 1);
	setVisibility(vehicle, componentIndex, 0);
}

void setVehicleComponentFlags(CVehicle* vehicle, string component, unsigned int flags) {
	if (vehicle) {
		RwFrame* frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, component.c_str());
		if (frame) {
			CVehicleModelInfo* mi = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));
			mi->SetVehicleComponentFlags(frame, flags);
		}
	}
}
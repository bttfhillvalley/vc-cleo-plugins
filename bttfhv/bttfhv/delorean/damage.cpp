#include "../vehicle/components.h"
#include "../rw/utils.h"
#include "CClumpModelInfo.h"
#include "CModelInfo.h"
#include "../constants.h"
#include "CTimer.h"
#include "CCamera.h"
#include "CWorld.h"
#include <iostream>

#include "delorean.h"

#define NUMTEMPOBJECTS 40
enum {
	MI_CAR_DOOR = 240,
	MI_CAR_BUMPER,
	MI_CAR_PANEL,
	MI_CAR_BONNET,
	MI_CAR_BOOT,
	MI_CAR_WHEEL,
};

vector<RwFrame*> Delorean::GetFrames(vector<string> components) {
	vector <RwFrame*> frames;
	RwFrame* frame;
	for (auto c : components) {
		frame = CClumpModelInfo::GetFrameFromName(automobile->m_pRwClump, c.c_str());
		if (frame) {
			cout << "Cannot find " << c << endl;
			frames.push_back(frame);
		}
	}
	return frames;
}

void Delorean::LoadFrames() {
	bonnetFrames = GetFrames(BONNET_COMPONENTS);
	bootFrames = GetFrames(BOOT_COMPONENTS);
	doorLeftFrontFrames = GetFrames(DOOR_LF_COMPONENTS);
	doorRightFrontFrames = GetFrames(DOOR_RF_COMPONENTS);
	bumpFrontFrames = GetFrames(BUMP_FRONT_COMPONENTS);
	bumpRearFrames = GetFrames(BUMP_REAR_COMPONENTS);
	windscreenFrames = GetFrames(WINDSCREEN_COMPONENTS);
	wingLeftFrontFrames = GetFrames(WING_LF_COMPONENTS);
	wingLeftRearFrames = GetFrames(WING_LR_COMPONENTS);
	wingRightFrontFrames = GetFrames(WING_RF_COMPONENTS);
	wingRightRearFrames = GetFrames(WING_RR_COMPONENTS);
	framesLoaded = true;
}

void Delorean::processDoorDamage(eDoors door, vector<RwFrame*> *frames) {
	unsigned int status = automobile->m_carDamage.GetDoorStatus(door);
	bool ok, dam;
	char* name;
	if (doorStatus[door] != status) {
		doorStatus[door] = status;
		if (!framesLoaded) {
			LoadFrames();
		}
		if (status == 0 || status == 2) {
			return;
		}

		ok = status == 0;
		dam = status == 1;
		for (auto f = frames->begin(); f != frames->end(); ++f) {
			name = GetFrameNodeName(*f);
			if (strstr(name, "_fr") != NULL) {
				continue;
			}
			if (strstr(name, "hi_ok") != NULL) {
				setVisibility(automobile, name, ok);
			}
			else if (strstr(name, "hi_dam") != NULL) {
				setVisibility(automobile, name, dam);
				if (status == 3) {
					SpawnFlyingComponent(*f, door == BONNET ? COMPGROUP_BONNET : door == BOOT ? COMPGROUP_BOOT : COMPGROUP_DOOR);
				}
			}
		}

	}
}

void Delorean::processPanelDamage(ePanels panel, vector<RwFrame*> *frames) {
	unsigned int status = automobile->m_carDamage.GetPanelStatus(panel);
	bool ok, dam;
	char* name;
	if (panelStatus[panel] != status) {
		if (!framesLoaded) {
			LoadFrames();
		}
		panelStatus[panel] = status;
		ok = status == 0;
		dam = status == 1 || status == 2;
		for (auto f = frames->begin(); f != frames->end(); ++f) {
			name = GetFrameNodeName(*f);
			if (strstr(name, "_fr") != NULL) {
				continue;
			}
			if (strstr(name, "hi_ok") != NULL) {
				setVisibility(automobile, name, ok);
			}
			else if (strstr(name, "hi_dam") != NULL) {
				setVisibility(automobile, name, dam);
				if (status == 3) {
					SpawnFlyingComponent(*f, COMPGROUP_PANEL);
				}
			}
		}
	}
}

void Delorean::ProcessDamage() {
	processDoorDamage(BONNET, &bonnetFrames);
	processDoorDamage(BOOT, &bootFrames);
	processDoorDamage(DOOR_FRONT_LEFT, &doorLeftFrontFrames);
	processDoorDamage(DOOR_FRONT_RIGHT, &doorRightFrontFrames);
	processPanelDamage(BUMP_FRONT, &bumpFrontFrames);
	processPanelDamage(BUMP_REAR, &bumpRearFrames);
	processPanelDamage(WINDSCREEN, &windscreenFrames);
	processPanelDamage(WING_FRONT_LEFT, &wingLeftFrontFrames);
	processPanelDamage(WING_REAR_LEFT, &wingLeftRearFrames);
	processPanelDamage(WING_FRONT_RIGHT, &wingRightFrontFrames);
	processPanelDamage(WING_REAR_RIGHT, &wingRightRearFrames);
}

CObject* Delorean::SpawnFlyingComponent(RwFrame *frame, unsigned int type)
{
	RpAtomic* atomic;
	RwMatrix* matrix;
	CObject* obj;

	if (CObject::nNoTempObjects >= NUMTEMPOBJECTS)
		return NULL;
	if (frame == NULL) {
		return NULL;
	}
	atomic = NULL;
	RwFrameForAllObjects(frame, GetAtomicObjectCB, &atomic);
	if (atomic == NULL)
		return NULL;

	obj = new CObject();
	if (obj == NULL)
		return NULL;

	string component = GetFrameNodeName(frame);
	if (component.starts_with("windscreen")) {
		obj->SetModelIndexNoCreate(MI_CAR_BONNET);
	}
	else switch (type) {
	case COMPGROUP_BUMPER:
		obj->SetModelIndexNoCreate(MI_CAR_BUMPER);
		break;
	case COMPGROUP_WHEEL:
		obj->SetModelIndexNoCreate(MI_CAR_WHEEL);
		break;
	case COMPGROUP_DOOR:
		obj->SetModelIndexNoCreate(MI_CAR_DOOR);
		obj->SetCenterOfMass(0.0f, -0.5f, 0.0f);
		obj->m_nFlags.bDrawLast = true;
		break;
	case COMPGROUP_BONNET:
		obj->SetModelIndexNoCreate(MI_CAR_BONNET);
		obj->SetCenterOfMass(0.0f, 0.4f, 0.0f);
		break;
	case COMPGROUP_BOOT:
		obj->SetModelIndexNoCreate(MI_CAR_BOOT);
		obj->SetCenterOfMass(0.0f, -0.3f, 0.0f);
		break;
	case COMPGROUP_PANEL:
	default:
		obj->SetModelIndexNoCreate(MI_CAR_PANEL);
		break;
	}

	// object needs base model
	obj->RefModelInfo(automobile->m_nModelIndex);

	// create new atomic
	matrix = RwFrameGetLTM(frame);
	frame = RwFrameCreate();
	atomic = RpAtomicClone(atomic);
	*RwFrameGetMatrix(frame) = *matrix;
	RpAtomicSetFrame(atomic, frame);
	CClumpModelInfo::SetAtomicRendererCB(atomic, NULL);
	obj->AttachToRwObject((RwObject*)atomic);
	obj->m_nFlags.bDontStream = true;

	// init object
	obj->m_fMass = 10.0f;
	obj->m_fTurnMass = 25.0f;
	obj->m_fAirResistance = 0.97f;
	obj->m_fElasticity = 0.1f;
	obj->m_fBuoyancyConstant = obj->m_fMass * GRAVITY / 0.75f;
	obj->m_nObjectType = TEMP_OBJECT;
	obj->m_nFlags.bIsStatic = false;
	obj->m_nObjectFlags.bIsPickupObject = false;
	obj->m_nObjectFlags.bIsVehicleComponent = true;
	obj->m_nCarColor[0] = automobile->m_nPrimaryColor;
	obj->m_nCarColor[1] = automobile->m_nSecondaryColor;

	// life time - the more objects the are, the shorter this one will live
	CObject::nNoTempObjects++;
	if (CObject::nNoTempObjects > 20)
		obj->m_dwObjectTimer = CTimer::GetCurrentTimeInCycles() + 20000 / 5.0f;
	else if (CObject::nNoTempObjects > 10)
		obj->m_dwObjectTimer = CTimer::GetCurrentTimeInCycles() + 20000 / 2.0f;
	else
		obj->m_dwObjectTimer = CTimer::GetCurrentTimeInCycles() + 20000;

	obj->m_vecMoveSpeed = automobile->m_vecMoveSpeed;
	if (obj->m_vecMoveSpeed.z > 0.0f) {
		obj->m_vecMoveSpeed.z *= 1.5f;
	}
	else if (automobile->m_placement.at.z > 0.0f &&
		(component.starts_with("bonnet") || component.starts_with("boot") || component.starts_with("windscreen"))) {
		obj->m_vecMoveSpeed.z *= -1.5f;
		obj->m_vecMoveSpeed.z += 0.04f;
	}
	else {
		obj->m_vecMoveSpeed.z *= 0.25f;
	}
	obj->m_vecMoveSpeed.x *= 0.75f;
	obj->m_vecMoveSpeed.y *= 0.75f;

	obj->m_vecTurnSpeed = automobile->m_vecTurnSpeed * 2.0f;

	// push component away from car
	CVector dist = obj->GetPosition() - automobile->m_placement.pos;
	dist.Normalise();
	if (component.starts_with("bonnet") || component.starts_with("boot") || component.starts_with("windscreen")) {
		// push these up some
		dist += automobile->m_placement.at;
		if (automobile->m_placement.at.z > 0.0f) {
			// simulate fast upward movement if going fast
			float speed = CVector2D(automobile->m_vecMoveSpeed.x, automobile->m_vecMoveSpeed.y).Magnitude();
			obj->m_placement.Translate(automobile->m_placement.at * speed);
		}
	}
	obj->ApplyMoveForce(dist);

	if (type == COMPGROUP_WHEEL) {
		obj->m_fTurnMass = 5.0f;
		obj->m_vecTurnSpeed.x = 0.5f;
		obj->m_fAirResistance = 0.99f;
	}

	if (automobile->m_nState == STATUS_WRECKED && automobile->IsVisible() && DotProduct(dist, TheCamera.pos - automobile->m_placement.pos) > -0.5f) {
		dist = TheCamera.pos - automobile->m_placement.pos;
		dist.Normalise();
		dist.z += 0.3f;
		automobile->ApplyMoveForce(5.0f * dist);
	}

	obj->m_pContactPhysical = automobile;

	if (automobile->m_nFlags.bRenderScorched)
		obj->m_nFlags.bRenderScorched = true;

	CWorld::Add(obj);

	return obj;
}
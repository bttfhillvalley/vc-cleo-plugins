#define _USE_MATH_DEFINES
#include <cmath>

#include "CTimer.h"
#include "eEntityStatus.h"

#include "../configuration/config.h"
#include "../constants.h"
#include "../utils/math.h"
#include "hover.h"


int HoverControl(CVehicle* vehicle, bool boost, bool landing, bool damaged)
{
	if (vehicle->m_pFlyingHandling == nullptr)
		return false;
	float fHoverState = 0.0f;
	float fHover = 0.0f;
	float fThrust = vehicle->m_pHandlingData->m_transmissionData.m_fEngineAcceleration * 5.0f;
	float fThrustFallOff = 0.5f / vehicle->m_pHandlingData->m_transmissionData.m_fMaxGearVelocity;
	float fThrustFallOffBack = -1.0f / vehicle->m_pHandlingData->m_transmissionData.m_fMinGearVelocity;
	float fPitch = 0.0f;
	float fRoll = 0.0f;
	float fYaw = 0.0f;
	float fUp = 1.0f;
	float fLandingSpeed = 0.1f;
	float fPedalState = 0.0f;
	float fTriggerState = 0.0f;
	float fThrustAccel = 0.0f;
	bool bBoostState = false;
	if (landing) {
		fUp -= fLandingSpeed;
	}
	tFlyingHandlingData* flyingHandling = vehicle->m_pFlyingHandling;
	float rm = pow(flyingHandling->fMoveRes, CTimer::ms_fTimeStep);
	float fUpSpeed = DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.at);
	float fForwardSpeed = DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.up);
	float fAttitude = asin(vehicle->m_placement.up.z);
	float fAttitudeUp = fAttitude + radians(90.0f);
	float fHeading = atan2(vehicle->m_placement.up.y, vehicle->m_placement.up.x);

	if (!CPad::GetPad(0)->DisablePlayerControls && (FindPlayerVehicle() == vehicle || vehicle->m_nState == STATUS_PLAYER_REMOTE)) {
		fPedalState = (CPad::GetPad(0)->GetAccelerate() - CPad::GetPad(0)->GetBrake()) / 255.0f;
		fTriggerState = (CPad::GetPad(0)->GetAccelerate() + CPad::GetPad(0)->GetBrake()) / 255.0f;
		bBoostState = (CPad::GetPad(0)->GetHandBrake() && fPedalState > 0.6f) || fTriggerState == 2.0f;
		if (fTriggerState == 2.0f) {
			fPedalState = 1.0f;
		}
		if (fForwardSpeed > 0.0f || fPedalState > 0.0f) {
			fThrustAccel = (fPedalState - fThrustFallOff * fForwardSpeed) * fThrust;
		}
		else {
			fThrustAccel = (fPedalState - fThrustFallOffBack * fForwardSpeed) * fThrust;
		}

		// Handle Vent Boost
		if (fPedalState > 0.0f && bBoostState && !landing) {
			fThrustAccel *= 0.8f;
		} else {
			fThrustAccel *= 0.3f;
		}

		fPitch = CPad::GetPad(0)->GetSteeringUpDown() / 128.0f;
		fHoverState = (float)((GetKeyState(configKeys.at("KEY_HOVERUP")) & 0x8000) != 0) - (float)((GetKeyState(configKeys.at("KEY_HOVERDN")) & 0x8000) != 0);
		if (CPad::GetPad(0)->PCTempJoyState.RightStickY == CPad::GetPad(0)->GetCarGunUpDown() && abs(CPad::GetPad(0)->PCTempJoyState.RightStickY) > 1.0f) {
			fHoverState = CPad::GetPad(0)->GetCarGunUpDown() / 128.0f;
		}
		else if (abs(CPad::GetPad(0)->LookAroundUpDown()) > 1.0f) {
			fPitch = CPad::GetPad(0)->LookAroundUpDown() / 128.0f;
		}
		fRoll = -CPad::GetPad(0)->GetSteeringLeftRight() / 128.0f;
		fYaw = CPad::GetPad(0)->GetCarGunLeftRight() / 128.0f;

		if (landing) {
			if (vehicle->m_placement.at.z > 0.0f) {
				fHover = Clamp(fHoverState, -1.0f * abs(cos(fAttitude)), 0.0f);
			}
			else {
				fHover = Clamp(fHoverState, 0.0f, 1.0f * abs(cos(fAttitude)));
			}
		}
		else {
			fHover = fHoverState;
		}
	}
	else if (vehicle->m_nState == STATUS_PHYSICS) {
		if (landing) {
			fHover = 0.0f;
		}
		else {
			fHover = -fLandingSpeed / flyingHandling->fThrust;
		}
		fYaw = 0.0f;
		fPitch = Clamp(0.5f * DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.up), -0.1f, 0.1f);
		fRoll = Clamp(0.5f * -vehicle->m_placement.right.z, -0.1f, 0.1f);
		fThrustAccel = 0.0f;
		rm *= 0.97f;
	}
	vehicle->m_vecMoveSpeed *= rm;
	if (!damaged) {
		fHover = flyingHandling->fThrust * fHover;
		if (vehicle->GetPosition().z > 1000.0f)
			fHover *= 10.0f / (vehicle->GetPosition().z - 70.0f);
	}
	else {
		fPitch = Clamp(fPitch * Clamp(5.0f - abs(vehicle->m_vecTurnSpeed.x), 0.0f, 5.0f) / 5.0f, -0.5f, 0.5f);
		fRoll = Clamp(fRoll * Clamp(5.0f - abs(vehicle->m_vecTurnSpeed.y), 0.0f, 5.0f) / 5.0f, -0.5f, 0.5f);
		fYaw = Clamp(fYaw * Clamp(5.0f - abs(vehicle->m_vecTurnSpeed.z), 0.0f, 5.0f) / 5.0f, -0.5f, 0.5f);
	}

	if (vehicle->m_nState == STATUS_PLAYER || vehicle->m_nState == STATUS_PLAYER_REMOTE || vehicle->m_nState == STATUS_PHYSICS) {
		// Hover
		CVector upVector(cos(fAttitudeUp) * cos(fHeading), cos(fAttitudeUp) * sin(fHeading), sin(fAttitudeUp));
		upVector.Normalise();
		if (!damaged) {
			float fLiftSpeed = DotProduct(vehicle->m_vecMoveSpeed, upVector);
			fUp -= flyingHandling->fThrustFallOff * fLiftSpeed;
		}
		else {
			fUp = 0.4f;
		}
		fUp *= cos(fAttitude);

		// Antigravity + Hover
		vehicle->ApplyMoveForce(GRAVITY * upVector * fUp * vehicle->m_fMass * CTimer::ms_fTimeStep);
		vehicle->ApplyMoveForce(GRAVITY * vehicle->m_placement.at * fHover * vehicle->m_fMass * CTimer::ms_fTimeStep);

		// Forward thrust
		vehicle->ApplyMoveForce(fThrustAccel * vehicle->m_placement.up * vehicle->m_fMass * CTimer::ms_fTimeStep);

		CVector forwardVector = vehicle->m_placement.up * (DotProduct(vehicle->m_placement.at, vehicle->m_placement.up) / powf(vehicle->m_placement.up.Magnitude(), 2.0f));
		CVector rightVector = vehicle->m_placement.at - forwardVector;

		// Get rid of any z thrust
		rightVector.z = 0.0f;
		rightVector *= flyingHandling->fThrust;

		// Limit sideways motion at higher speeds
		rightVector *= powf(M_E, -2.0f * vehicle->m_vecMoveSpeed.Magnitude());
		vehicle->ApplyMoveForce(GRAVITY * rightVector * vehicle->m_fMass * CTimer::ms_fTimeStep);
	} else if (abs(vehicle->m_vecMoveSpeed.z) < 0.1f) {
		// Don't allow any movement if on the ground and we aren't making any hover thrust
		fPitch = 0.0f;
		fRoll = 0.0f;
		fYaw = 0.0f;
	}

	if (vehicle->m_placement.at.z > 0.0f) {
		float upRight = Clamp(vehicle->m_placement.right.z, -flyingHandling->fFormLift, flyingHandling->fFormLift);
		float upImpulseRight = -upRight * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseRight * vehicle->m_placement.at, vehicle->m_placement.right);

		float upFwd = Clamp(vehicle->m_placement.up.z, -flyingHandling->fFormLift, flyingHandling->fFormLift);
		float upImpulseFwd = -upFwd * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseFwd * vehicle->m_placement.at, vehicle->m_placement.up);
	}
	else {
		float upRight = vehicle->m_placement.right.z < 0.0f ? -flyingHandling->fFormLift : flyingHandling->fFormLift;
		float upImpulseRight = -upRight * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseRight * vehicle->m_placement.at, vehicle->m_placement.right);

		float upFwd = vehicle->m_placement.up.z < 0.0f ? -flyingHandling->fFormLift : flyingHandling->fFormLift;
		float upImpulseFwd = -upFwd * flyingHandling->fAttackLift * vehicle->m_fTurnMass * CTimer::ms_fTimeStep;
		vehicle->ApplyTurnForce(upImpulseFwd * vehicle->m_placement.at, vehicle->m_placement.up);
	}

	vehicle->ApplyTurnForce(fPitch * vehicle->m_placement.at * flyingHandling->fPitch * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, vehicle->m_placement.up);
	vehicle->ApplyTurnForce(fRoll * vehicle->m_placement.at * flyingHandling->fRoll * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, vehicle->m_placement.right);

	float fSideSpeed = -DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.right);
	float fSideSlipAccel = flyingHandling->fSideSlip * fSideSpeed * abs(fSideSpeed);
	vehicle->ApplyMoveForce(vehicle->m_fMass * vehicle->m_placement.right * fSideSlipAccel * CTimer::ms_fTimeStep);

	float fSideSpeedB = -DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.at);
	float fSideSlipAccelB = flyingHandling->fSideSlip * fSideSpeedB * abs(fSideSpeedB);
	vehicle->ApplyMoveForce(vehicle->m_fMass * vehicle->m_placement.at * fSideSlipAccelB * CTimer::ms_fTimeStep);

	if (landing) {
		float fSideSpeedC = -DotProduct(vehicle->m_vecMoveSpeed, vehicle->m_placement.up);
		float fSideSlipAccelC = flyingHandling->fSideSlip * fSideSpeedC * abs(fSideSpeedC);
		fSideSlipAccelC /= 20.0f;
		vehicle->ApplyMoveForce(vehicle->m_fMass * vehicle->m_placement.up * fSideSlipAccelC * CTimer::ms_fTimeStep);
	}

	float fYawAccel = flyingHandling->fYawStab * fSideSpeed * abs(fSideSpeed) + flyingHandling->fYaw * fYaw;
	vehicle->ApplyTurnForce(fYawAccel * vehicle->m_placement.right * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, -vehicle->m_placement.up);
	vehicle->ApplyTurnForce(fYaw * vehicle->m_placement.up * flyingHandling->fYaw * vehicle->m_fTurnMass * CTimer::ms_fTimeStep, vehicle->m_placement.right);
	if (!damaged) {
		float rX = pow(flyingHandling->vecTurnRes.x, CTimer::ms_fTimeStep);
		float rY = pow(flyingHandling->vecTurnRes.y, CTimer::ms_fTimeStep);
		float rZ = pow(flyingHandling->vecTurnRes.z, CTimer::ms_fTimeStep);
		CVector vecTurnSpeed = Multiply3x3(vehicle->m_vecTurnSpeed, vehicle->m_placement);

		float fResistanceMultiplier = powf(1.0f / (flyingHandling->vecSpeedRes.z * SQR(vecTurnSpeed.z) + 1.0f) * rZ, CTimer::ms_fTimeStep);
		float fResistance = vecTurnSpeed.z * fResistanceMultiplier - vecTurnSpeed.z;
		vecTurnSpeed.x *= rX;
		vecTurnSpeed.y *= rY;
		vecTurnSpeed.z *= fResistanceMultiplier;
		vehicle->m_vecTurnSpeed = Multiply3x3(vehicle->m_placement, vecTurnSpeed);
		vehicle->ApplyTurnForce(-vehicle->m_placement.right * fResistance * vehicle->m_fTurnMass, vehicle->m_placement.up + Multiply3x3(vehicle->m_placement, vehicle->m_vecCentreOfMass));

		if (FindPlayerVehicle() == vehicle) {
			CPlayerInfo* player = FindPlayerPed()->GetPlayerInfoForThisPlayerPed();
			player->m_nUpsideDownCounter = 0;
		}
	}
	return ((fHoverState > 0.8f && vehicle->m_placement.at.z > 0.0f && !landing && !damaged) ? HOVER_UP : HOVER_NONE) |
		((fPedalState > 0.0f && bBoostState && boost && !landing && !damaged) ? HOVER_BOOST : HOVER_NONE);
}

void FixHoverPadShake(CAutomobile* automobile) {
	for (int i = 0; i < 4; i++) {
		if (automobile->m_carDamage.GetWheelStatus(i) == 2) {
			automobile->stWheels[i].surfaceB = SURFACE_TARMAC;
		}
	}
}
#pragma once
#include "plugin.h"
#include "VC.CLEO.h"
#pragma comment(lib, "VC.CLEO.lib")
#include "extensions\ScriptCommands.h"

using namespace plugin;
using namespace std;

extern tScriptVar* Params;

// Math
eOpcodeResult __stdcall getArcsin(CScript* script);
eOpcodeResult __stdcall getArccos(CScript* script);
eOpcodeResult __stdcall getTan(CScript* script);
eOpcodeResult __stdcall getArctan(CScript* script);

// Car
eOpcodeResult __stdcall raiseFrontSuspension(CScript* script);
eOpcodeResult __stdcall updateHandling(CScript* script);
eOpcodeResult __stdcall turnOnEngine(CScript* script);
eOpcodeResult __stdcall getEngineStatus(CScript* script);
eOpcodeResult __stdcall getCurrentGear(CScript* script);
eOpcodeResult __stdcall setHover(CScript* script);
eOpcodeResult __stdcall isDoorOpen(CScript* script);
eOpcodeResult __stdcall getDoorAngle(CScript* script);
eOpcodeResult __stdcall getWheelAngle(CScript* script);
eOpcodeResult __stdcall setWheelAngle(CScript* script);
eOpcodeResult __stdcall getWheelSpeed(CScript* script);
eOpcodeResult __stdcall setWheelSpeed(CScript* script);
eOpcodeResult __stdcall getDriveWheelsOnGround(CScript* script);
eOpcodeResult __stdcall isWheelsOnGround(CScript* script);
eOpcodeResult __stdcall isWheelsNotOnGround(CScript* script);
eOpcodeResult __stdcall setCarStatus(CScript* script);
eOpcodeResult __stdcall getCarStatus(CScript* script);
eOpcodeResult __stdcall setCarLights(CScript* script);
eOpcodeResult __stdcall getCarLights(CScript* script);
eOpcodeResult __stdcall setCarComponentVisibility(CScript* script);
eOpcodeResult __stdcall isCarComponentVisible(CScript* script);
eOpcodeResult __stdcall isCarComponentNotVisible(CScript* script);
eOpcodeResult __stdcall isCarComponentIndexVisible(CScript* script);
eOpcodeResult __stdcall isCarComponentIndexNotVisible(CScript* script);
eOpcodeResult __stdcall setCarComponentGlow(CScript* script);
eOpcodeResult __stdcall setCarComponentGlowIndex(CScript* script);
eOpcodeResult __stdcall setCarComponentIndexVisibility(CScript* script);
eOpcodeResult __stdcall setCarComponentColor(CScript* script);
eOpcodeResult __stdcall setCarComponentIndexColor(CScript* script);
eOpcodeResult __stdcall setCarComponentAlpha(CScript* script);
eOpcodeResult __stdcall setCarComponentIndexAlpha(CScript* script);
eOpcodeResult __stdcall fadeCarComponentAlpha(CScript* script);
eOpcodeResult __stdcall fadeCarComponentIndexAlpha(CScript* script);
eOpcodeResult __stdcall carComponentDigitOff(CScript* script);
eOpcodeResult __stdcall carComponentDigitOn(CScript* script);
eOpcodeResult __stdcall carComponentDigitGet(CScript* script);
eOpcodeResult __stdcall getCarComponentAlpha(CScript* script);
eOpcodeResult __stdcall getCarComponentIndexAlpha(CScript* script);
eOpcodeResult __stdcall moveCarComponent(CScript* script);
eOpcodeResult __stdcall moveCarComponentIndex(CScript* script);
eOpcodeResult __stdcall getCarComponentPosition(CScript* script);
eOpcodeResult __stdcall getCarComponentOffset(CScript* script);
eOpcodeResult __stdcall rotateCarComponent(CScript* script);
eOpcodeResult __stdcall rotateCarComponentIndex(CScript* script);
eOpcodeResult __stdcall setCarComponentFlags(CScript* script);
eOpcodeResult __stdcall setCarComponentFlagsIndex(CScript* script);
eOpcodeResult __stdcall getCarComponentRotation(CScript* script);
eOpcodeResult __stdcall rotateBonnet(CScript* script);
eOpcodeResult __stdcall rotateBoot(CScript* script);
eOpcodeResult __stdcall getGasPedalAudio(CScript* script);
eOpcodeResult __stdcall getCarOrientation(CScript* script);
eOpcodeResult __stdcall setCarOrientation(CScript* script);
eOpcodeResult __stdcall popWheelie(CScript* script);
eOpcodeResult __stdcall skiMode(CScript* script);
eOpcodeResult __stdcall rotateCar(CScript* script);
eOpcodeResult __stdcall getCarRotation(CScript* script);
eOpcodeResult __stdcall setRemote(CScript* script);
eOpcodeResult __stdcall removeRemote(CScript* script);
eOpcodeResult __stdcall inRemote(CScript* script);
eOpcodeResult __stdcall applyForwardForce(CScript* script);
eOpcodeResult __stdcall applyUpwardForce(CScript* script);
eOpcodeResult __stdcall getWheelStatus(CScript* script);
eOpcodeResult __stdcall setWheelStatus(CScript* script);
eOpcodeResult __stdcall wheelSparks(CScript* script);
eOpcodeResult __stdcall setCarCollision(CScript* script);
eOpcodeResult __stdcall setCarEngineSound(CScript* script);
eOpcodeResult __stdcall getVehicleFlags(CScript* script);
eOpcodeResult __stdcall setVehicleFlags(CScript* script);
eOpcodeResult __stdcall getRotationMatrix(CScript* script);
eOpcodeResult __stdcall setRotationMatrix(CScript* script);
eOpcodeResult __stdcall getUpMatrix(CScript* script);
eOpcodeResult __stdcall getAtMatrix(CScript* script);
eOpcodeResult __stdcall getRightMatrix(CScript* script);
eOpcodeResult __stdcall getVelocityVector(CScript* script);
eOpcodeResult __stdcall setVelocityVector(CScript* script);
eOpcodeResult __stdcall getVelocityDirection(CScript* script);
eOpcodeResult __stdcall getVelocity(CScript* script);
eOpcodeResult __stdcall setVelocity(CScript* script);
eOpcodeResult __stdcall getForwardVelocityVectorWithSpeed(CScript* script);
eOpcodeResult __stdcall getRelativeVelocity(CScript* script);
eOpcodeResult __stdcall setGasPedal(CScript* script);
eOpcodeResult __stdcall setBrakePedal(CScript* script);
eOpcodeResult __stdcall setHandBrake(CScript* script);
eOpcodeResult __stdcall setSteeringAngle(CScript* script);
eOpcodeResult __stdcall getSteeringAngle(CScript* script);
eOpcodeResult __stdcall createLight(CScript* script);
eOpcodeResult __stdcall createParticle(CScript* script);
eOpcodeResult __stdcall isAttached(CScript* script);
eOpcodeResult __stdcall attachVehicle(CScript* script);
eOpcodeResult __stdcall detachVehicle(CScript* script);
eOpcodeResult __stdcall replaceTex(CScript* script);

// Building
eOpcodeResult __stdcall addBuilding(CScript* script);
eOpcodeResult __stdcall removeBuilding(CScript* script);
eOpcodeResult __stdcall addObjects(CScript* script);
eOpcodeResult __stdcall removeObjects(CScript* script);
eOpcodeResult __stdcall setBuildingComponentVisibility(CScript* script);

// Animation
eOpcodeResult __stdcall playCharAnim(CScript* script);
eOpcodeResult __stdcall loadTxdDict(CScript* script);

// Sound
eOpcodeResult __stdcall stopAllSounds(CScript* script);
eOpcodeResult __stdcall stopSound(CScript* script);
eOpcodeResult __stdcall stopSoundIndex(CScript* script);
eOpcodeResult __stdcall isSoundPlaying(CScript* script);
eOpcodeResult __stdcall isSoundStopped(CScript* script);
eOpcodeResult __stdcall isSoundPlayingIndex(CScript* script);
eOpcodeResult __stdcall isSoundStoppedIndex(CScript* script);
eOpcodeResult __stdcall playSound(CScript* script);
eOpcodeResult __stdcall playSoundIndex(CScript* script);
eOpcodeResult __stdcall playKeypad(CScript* script);
eOpcodeResult __stdcall playSoundAtLocation(CScript* script);
eOpcodeResult __stdcall playSoundAtLocationIndex(CScript* script);
eOpcodeResult __stdcall attachSoundToVehicle(CScript* script);
eOpcodeResult __stdcall setFrequency(CScript* script);
eOpcodeResult __stdcall setVolume(CScript* script);
eOpcodeResult __stdcall attachSoundToVehicleIndex(CScript* script);
eOpcodeResult __stdcall setDoppler(CScript* script);
eOpcodeResult __stdcall setReverb(CScript* script);
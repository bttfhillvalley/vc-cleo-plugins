#include <iostream>
#include "plugin.h"
#include "irrKlang.h"
#include "VC.CLEO.h"
#pragma comment(lib, "VC.CLEO.lib")
#include "CCamera.h"
#include "CMenuManager.h"
#include "CWorld.h"

#include "constants.h"
#include "building\idemap.h"
#include "configuration\config.h"
#include "configuration\fileloader.h"
#include "opcodes\opcodes.h"
#include "rw\utils.h"
#include "sound\sound.h"
#include "utils\math.h"
#include "vehicle\attachment.h"
#include "vehicle\components.h"
#include "vehicle\handling.h"
#include "vehicle\hover.h"

#include "delorean\delorean.h"

tScriptVar* Params;

boolean paused = false;
bool saveSuccessful = false;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	CVector* pos;
	vec3df playerPos, soundPos, soundVel, dir;
	string key;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE)hModule);
		Params = CLEO_GetParamsAddress();
		Opcodes::RegisterOpcode(0x3F01, raiseFrontSuspension);
		Opcodes::RegisterOpcode(0x3F02, getEngineStatus);
		Opcodes::RegisterOpcode(0x3F03, turnOnEngine);
		Opcodes::RegisterOpcode(0x3F04, getCurrentGear);
		Opcodes::RegisterOpcode(0x3F05, setHover);
		Opcodes::RegisterOpcode(0x3F06, isDoorOpen);
		Opcodes::RegisterOpcode(0x3F07, playCharAnim);
		Opcodes::RegisterOpcode(0x3F08, addBuilding);
		Opcodes::RegisterOpcode(0x3F09, removeBuilding);
		Opcodes::RegisterOpcode(0x3F0A, replaceTex);
		Opcodes::RegisterOpcode(0x3F0B, replaceTexIndex);
		Opcodes::RegisterOpcode(0x3F0C, setCarCollision);
		Opcodes::RegisterOpcode(0x3F0D, getDoorAngle);
		Opcodes::RegisterOpcode(0x3F0E, getWheelAngle);
		Opcodes::RegisterOpcode(0x3F0F, getWheelSpeed);
		Opcodes::RegisterOpcode(0x3F10, setCarComponentVisibility);
		Opcodes::RegisterOpcode(0x3F11, setCarComponentIndexVisibility);
		Opcodes::RegisterOpcode(0x3F12, setCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3F13, setCarComponentIndexAlpha);
		Opcodes::RegisterOpcode(0x3F14, moveCarComponent);
		Opcodes::RegisterOpcode(0x3F15, moveCarComponentIndex);
		Opcodes::RegisterOpcode(0x3F16, rotateCarComponent);
		Opcodes::RegisterOpcode(0x3F17, rotateCarComponentIndex);
		Opcodes::RegisterOpcode(0x3F18, setCarComponentGlow);
		Opcodes::RegisterOpcode(0x3F19, setCarComponentGlowIndex);
		Opcodes::RegisterOpcode(0x3F1A, getGasPedalAudio);
		Opcodes::RegisterOpcode(0x3F1B, getCarComponentPosition);
		Opcodes::RegisterOpcode(0x3F1C, getCarComponentRotation);
		Opcodes::RegisterOpcode(0x3F1D, getCarComponentOffset);
		Opcodes::RegisterOpcode(0x3F1E, setWheelAngle);
		Opcodes::RegisterOpcode(0x3F1F, setWheelSpeed);
		Opcodes::RegisterOpcode(0x3F20, getCarOrientation);
		Opcodes::RegisterOpcode(0x3F21, setCarOrientation);
		Opcodes::RegisterOpcode(0x3F22, popWheelie);
		Opcodes::RegisterOpcode(0x3F23, setRemote);
		Opcodes::RegisterOpcode(0x3F24, removeRemote);
		Opcodes::RegisterOpcode(0x3F25, applyForwardForce);
		Opcodes::RegisterOpcode(0x3F26, applyUpwardForce);
		Opcodes::RegisterOpcode(0x3F27, getWheelStatus);
		Opcodes::RegisterOpcode(0x3F28, setWheelStatus);
		Opcodes::RegisterOpcode(0x3F29, createLight);
		Opcodes::RegisterOpcode(0x3F2A, inRemote);
		Opcodes::RegisterOpcode(0x3F2B, attachVehicle);
		Opcodes::RegisterOpcode(0x3F2C, detachVehicle);
		Opcodes::RegisterOpcode(0x3F2D, setCarEngineSound);
		Opcodes::RegisterOpcode(0x3F2E, getDriveWheelsOnGround);
		Opcodes::RegisterOpcode(0x3F2F, getCarRotation);
		Opcodes::RegisterOpcode(0x3F30, rotateCar);
		Opcodes::RegisterOpcode(0x3F31, getRotationMatrix);
		Opcodes::RegisterOpcode(0x3F32, setRotationMatrix);
		Opcodes::RegisterOpcode(0x3F33, getVelocityDirection);
		Opcodes::RegisterOpcode(0x3F34, getVelocityVector);
		Opcodes::RegisterOpcode(0x3F35, setVelocityVector);
		Opcodes::RegisterOpcode(0x3F36, getSteeringAngle);
		Opcodes::RegisterOpcode(0x3F37, addObjects);
		Opcodes::RegisterOpcode(0x3F38, removeObjects);
		Opcodes::RegisterOpcode(0x3F39, getVelocity);
		Opcodes::RegisterOpcode(0x3F3A, setVelocity);
		Opcodes::RegisterOpcode(0x3F3B, getForwardVelocityVectorWithSpeed);
		Opcodes::RegisterOpcode(0x3F3C, getRelativeVelocity);
		Opcodes::RegisterOpcode(0x3F3D, skiMode);
		Opcodes::RegisterOpcode(0x3F3E, isWheelsOnGround);
		Opcodes::RegisterOpcode(0x3F3F, isWheelsNotOnGround);
		Opcodes::RegisterOpcode(0x3F40, isCarComponentVisible);
		Opcodes::RegisterOpcode(0x3F41, isCarComponentIndexVisible);
		Opcodes::RegisterOpcode(0x3F42, getAtMatrix);
		Opcodes::RegisterOpcode(0x3F43, getRightMatrix);
		Opcodes::RegisterOpcode(0x3F44, getUpMatrix);
		Opcodes::RegisterOpcode(0x3F45, rotateBonnet);
		Opcodes::RegisterOpcode(0x3F46, rotateBoot);
		Opcodes::RegisterOpcode(0x3F47, getCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3F48, getCarComponentIndexAlpha);
		Opcodes::RegisterOpcode(0x3F49, setCarComponentColor);
		Opcodes::RegisterOpcode(0x3F4A, setCarComponentIndexColor);
		Opcodes::RegisterOpcode(0x3F4B, updateHandling);
		Opcodes::RegisterOpcode(0x3F4C, setBuildingComponentVisibility);
		Opcodes::RegisterOpcode(0x3F4E, setCarComponentFlags);
		Opcodes::RegisterOpcode(0x3F4F, setCarComponentFlagsIndex);
		Opcodes::RegisterOpcode(0x3F50, isCarComponentNotVisible);
		Opcodes::RegisterOpcode(0x3F51, isCarComponentIndexNotVisible);
		Opcodes::RegisterOpcode(0x3F52, fadeCarComponentAlpha);
		Opcodes::RegisterOpcode(0x3F53, fadeCarComponentIndexAlpha);
		Opcodes::RegisterOpcode(0x3F54, carComponentDigitOff);
		Opcodes::RegisterOpcode(0x3F55, carComponentDigitOn);
		Opcodes::RegisterOpcode(0x3F56, getArcsin);
		Opcodes::RegisterOpcode(0x3F57, getArccos);
		Opcodes::RegisterOpcode(0x3F58, getTan);
		Opcodes::RegisterOpcode(0x3F59, getArctan);
		Opcodes::RegisterOpcode(0x3F5A, getVehicleFlags);
		Opcodes::RegisterOpcode(0x3F5B, setVehicleFlags);
		Opcodes::RegisterOpcode(0x3F5C, setGasPedal);
		Opcodes::RegisterOpcode(0x3F5D, setBrakePedal);
		Opcodes::RegisterOpcode(0x3F5E, setSteeringAngle);
		Opcodes::RegisterOpcode(0x3F5F, setHandBrake);
		Opcodes::RegisterOpcode(0x3F60, createParticle);
		Opcodes::RegisterOpcode(0x3F61, wheelSparks);
		Opcodes::RegisterOpcode(0x3F62, createCarComponent);
		Opcodes::RegisterOpcode(0x3F64, carComponentDigitGet);
		Opcodes::RegisterOpcode(0x3F80, stopAllSounds);
		Opcodes::RegisterOpcode(0x3F81, stopSound);
		Opcodes::RegisterOpcode(0x3F82, isSoundPlaying);
		Opcodes::RegisterOpcode(0x3F83, isSoundStopped);
		Opcodes::RegisterOpcode(0x3F84, playSound);
		Opcodes::RegisterOpcode(0x3F85, playSoundAtLocation);
		Opcodes::RegisterOpcode(0x3F86, attachSoundToVehicle);
		Opcodes::RegisterOpcode(0x3F90, playKeypad);
		Opcodes::RegisterOpcode(0x3F91, stopSoundIndex);
		Opcodes::RegisterOpcode(0x3F92, isSoundPlayingIndex);
		Opcodes::RegisterOpcode(0x3F93, isSoundStoppedIndex);
		Opcodes::RegisterOpcode(0x3F94, playSoundIndex);
		Opcodes::RegisterOpcode(0x3F95, playSoundAtLocationIndex);
		Opcodes::RegisterOpcode(0x3F96, attachSoundToVehicleIndex);
		Opcodes::RegisterOpcode(0x3F97, setFrequency);
		Opcodes::RegisterOpcode(0x3F98, setVolume);
		Opcodes::RegisterOpcode(0x3F99, setDoppler);
		Opcodes::RegisterOpcode(0x3F9A, setCarStatus);
		Opcodes::RegisterOpcode(0x3F9B, getCarStatus);
		Opcodes::RegisterOpcode(0x3F9C, setCarLights);
		Opcodes::RegisterOpcode(0x3F9D, getCarLights);
		Opcodes::RegisterOpcode(0x3F9E, setReverb);
		Opcodes::RegisterOpcode(0x3F9F, setDoorStatus);

		//Opcodes::RegisterOpcode(0x3F37, replaceTex);
		//Opcodes::RegisterOpcode(0x3F38, addCompAnims);
		//Reserving 0x3F18-0x3F1F for get command

		Events::vehicleRenderEvent.before += [&](CVehicle* vehicle) {
			// Flying handling
			UpdateFlyingHandling(vehicle);

			// Delorean stuff
			Delorean* delorean;
			auto it = deloreanMap.find(vehicle);
			if (it == deloreanMap.end()) {
				if (vehicle->m_nState != STATUS_WRECKED  && (getVisibility(vehicle, "bttf1") || getVisibility(vehicle, "bttf2"))) {
					delorean = new Delorean(vehicle);
					delorean->Update();
					deloreanMap[vehicle] = delorean;
				}
			}
			else {
				delorean = it->second;
				if (delorean->IsWrecked()) {
					delete delorean;
					deloreanMap.erase(vehicle);
				}
				else {
					delorean->Update();
				}
			}

			// Car attachment
			for (auto const& [vehicle_id, attachment] : carAttachments) {
				if (attachment.attached == vehicle) {
					CMatrix matrix = attachment.vehicle->m_placement * attachment.offset;
					attachment.attached->m_placement.up = matrix.up;
					attachment.attached->m_placement.at = matrix.at;
					attachment.attached->m_placement.right = matrix.right;
					attachment.attached->m_placement.pos = matrix.pos;
					attachment.attached->m_vecMoveSpeed = attachment.vehicle->m_vecMoveSpeed;
				}
			}
		};
		Events::initGameEvent += [] {
			patch::SetChar(0x5921BC, 0x66, true); // Engine smoke in rear for Delorean
			patch::SetInt(0x5921BD, 0x00EDFA81, true);
			patch::Nop(0x54F429, 5, true); // Disable plane trails
			patch::Nop(0x58E59B, 5, true); // Disable Tail light point lights
			patch::Nop(0x58E611, 5, true); // Disable Brake light point lights
			patch::Nop(0x58CDA7, 13, true); // Disable engine check for lights

			patch::SetFloat(0x69C70C, 1000.0, true); // Change height limit for Delorean


			// Disable STATUS_ABANDONED overriding player controls
			patch::Nop(0x593F83, 10, true);
			patch::Nop(0x593F90, 10, true);
			patch::Nop(0x593FAA, 10, true);

			// These next ones disables the collapsing of frames on particular dummies
			patch::SetInt(0x699730, 0x80, true); // Front Bumper
			patch::SetInt(0x69973c, 0x00, true); // Bonnet
			patch::SetInt(0x699748, 0x00, true); // Wing RF
			patch::SetInt(0x699754, 0x40, true); // Wing RR
			patch::SetInt(0x699760, 0x5050, true); // Door RF
			//patch::SetInt(0x69976c, 0x3150, true); // Door RR
			patch::SetInt(0x699778, 0x00, true); // Wing LF
			patch::SetInt(0x699784, 0x20, true); // Wing LR
			patch::SetInt(0x699790, 0x5030, true); // Door LF
			//patch::SetInt(0x69979c, 0x3130, true); // Door LR
			patch::SetInt(0x6997a8, 0x100, true); // Boot
			//patch::SetInt(0x6997b4, 0x100, true); // Rear Bumper
			patch::SetInt(0x6997c0, 0xc80, true); // Windscreen
			//patch::SetInt(0x00579D3F size 5 value 0x90, true);
			//patch::SetInt(0x00579D4C size 5 value 0x90, true);

			// Delorean slot (237)
			patch::SetInt(0x6ADD54, 274, true);   // Acceleration Sample (SFX_CAR_REV_7)
			patch::SetInt(0x6ADD58, 10, true);    // Engine Bank (COBRA)
			patch::SetInt(0x6ADD5C, 1, true);     // Horn Sample (BMW328)
			patch::SetInt(0x6ADD60, 12017, true); // Horn Frequency
			patch::SetInt(0x6ADD64, 0, true);     // Alarm Sample (JEEP)
			patch::SetInt(0x6ADD68, 9900, true);  // Alarm Frequency
			patch::SetInt(0x6ADD6C, 1, true);     // Door Type (NEW)

			// Rogers slot (238)
			patch::SetInt(0x6ADD70, 283, true);   // Acceleration Sample (UNUSED)
			patch::SetInt(0x6ADD74, 19, true);    // Engine Bank (UNUSED)
			patch::SetInt(0x6ADD78, 37, true);    // Horn Sample (SFX_PALM_TREE_LO)
			patch::SetInt(0x6ADD7C, 12017, true); // Horn Frequency
			patch::SetInt(0x6ADD80, 0, true);     // Alarm Sample (JEEP)
			patch::SetInt(0x6ADD84, 9900, true);  // Alarm Frequency
			patch::SetInt(0x6ADD88, 3, true);     // Door Type (AIRBRAKES)

			// Rogers slot (239)
			patch::SetInt(0x6ADD8C, 283, true);   // Acceleration Sample (UNUSED)
			patch::SetInt(0x6ADD90, 19, true);    // Engine Bank (UNUSED)
			patch::SetInt(0x6ADD94, 37, true);    // Horn Sample (SFX_PALM_TREE_LO)
			patch::SetInt(0x6ADD98, 12017, true); // Horn Frequency
			patch::SetInt(0x6ADD9C, 0, true);     // Alarm Sample (JEEP)
			patch::SetInt(0x6ADDA0, 9900, true);  // Alarm Frequency
			patch::SetInt(0x6ADDA4, 3, true);     // Door Type (AIRBRAKES)

			if (!loadedSound) {
				m_soundEngine = createIrrKlangDevice();
				m_soundEngine->setRolloffFactor(1.5f);
				m_soundEngine->setDopplerEffectParameters(3.0f, 10.0f);
				loadedSound = true;
			}
			else {
				m_soundEngine->removeAllSoundSources();
			}
			soundMap.clear();
			handlingData.clear();
			bikeHandlingData.clear();
			boatHandlingData.clear();
			flyingHandlingData.clear();
			deloreanMap.clear();
			carAttachments.clear();
			ideMap.clear();
			removeObjectQueue.clear();
			keyMap.clear();
			configKeys.clear();
			InitializeKeyMap();
			LoadKeyConfig();
			LoadAdditionalHandlingData();
			LoadAdditionalVehicleColours();

			int millis = 60000;
			auto it = keyMap.find("MILLIS_IN_MINUTE");
			if (it != keyMap.end()) {
				millis = it->second;
			}
			patch::SetInt(0x97F2B4, millis, true); // Set real time
		};

		Events::initScriptsEvent += [] {
			UpdateHandling();
		};

		Events::gameProcessEvent += [&] {
			// Set volume of sound engine to match game
			if (volume != FrontendMenuManager.m_nPrefsSfxVolume) {
				volume = FrontendMenuManager.m_nPrefsSfxVolume;
				m_soundEngine->setSoundVolume(volume / 127.0f);
			}
			if (Command<Commands::IS_PLAYER_PLAYING>(0))
			{
				pos = TheCamera.GetGameCamPosition();
				playerPos.X = pos->x;
				playerPos.Y = -1.0f * pos->y;
				playerPos.Z = pos->z;
				dir.X = TheCamera.up.x;
				dir.Y = -1.0f * TheCamera.up.y;
				dir.Z = TheCamera.up.z;
				m_soundEngine->setListenerPosition(playerPos, dir, vec3df(0, 0, 0), vec3df(0, 0, 1));
			}
			if (FrontendMenuManager.m_bMenuActive) {
				if (FrontendMenuManager.m_nCurrentMenuPage == MENUPAGE_SAVE_SUCCESSFUL && !saveSuccessful) {
					saveSuccessful = true;
					cout << "BTTFHV: Saved to " << FrontendMenuManager.m_nCurrentSaveSlot << endl;
				}
				else if (FrontendMenuManager.m_nCurrentMenuPage != MENUPAGE_SAVE_SUCCESSFUL && saveSuccessful) {
					saveSuccessful = false;
				}
			}
			else if (saveSuccessful) {
				saveSuccessful = false;
			}

			if (!soundMap.empty()) {
				if (FrontendMenuManager.m_bMenuActive && !paused) {
					for (auto const& [key, gamesound] : soundMap) {
						gamesound.sound->setIsPaused();
					}
					paused = true;
				}
				else if (!FrontendMenuManager.m_bMenuActive) {
					auto itr = soundMap.begin();
					while (itr != soundMap.end()) {
						// Delete sound if its finished playing
						if (soundMap[itr->first].sound->isFinished()) {
							itr = soundMap.erase(itr);
							continue;
						}
						// Unpause sound if we're paused
						if (paused) {
							soundMap[itr->first].sound->setIsPaused(false);
						}
						if (soundMap[itr->first].vehicle) {
							// Stop sound if wrecked
							int index = CPools::GetVehicleRef(soundMap[itr->first].vehicle);
							if (index < 0 || soundMap[itr->first].vehicle->m_nState == STATUS_WRECKED || soundMap[itr->first].vehicle->m_nVehicleFlags.bIsDrowning) {
								soundMap[itr->first].sound->stop();
								soundMap[itr->first].sound->drop();
								itr = soundMap.erase(itr);
								continue;
							}
							// Attach sound to vehicle
							Command<Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(soundMap[itr->first].vehicle, soundMap[itr->first].offset.x, soundMap[itr->first].offset.y, soundMap[itr->first].offset.z, &soundPos.X, &soundPos.Y, &soundPos.Z);
							soundPos.Y *= -1.0;
							soundMap[itr->first].sound->setPosition(soundPos);
							// Set speed for doppler effect
							soundVel.X = soundMap[itr->first].vehicle->m_vecMoveSpeed.x;
							soundVel.Y = soundMap[itr->first].vehicle->m_vecMoveSpeed.y * -1.0f;
							soundVel.Z = soundMap[itr->first].vehicle->m_vecMoveSpeed.z;
							soundMap[itr->first].sound->setVelocity(soundVel);
						}
						else {
							// Set sound to specified location
							soundPos.X = soundMap[itr->first].offset.x;
							soundPos.Y = soundMap[itr->first].offset.y;
							soundPos.Z = soundMap[itr->first].offset.z;
						}

						// Mute sound if > 150 units away, otherwise play at full volume
						/*distance = (float)playerPos.getDistanceFrom(soundPos);
						if (distance < 150.0f || !soundMap[itr->first].spatial) {
							soundMap[itr->first].sound->setVolume(1.0f);
						}
						else {
							soundMap[itr->first].sound->setVolume(0.0f);
						}*/
						++itr;
					}
					paused = false;
				}
			}


			// Removes dynamically created objects.  Has to be here because game script tick causes them to flash briefly
			for (auto item : removeObjectQueue) {
				auto models = item.second;
				for (auto object : CPools::ms_pObjectPool) {
					if (models->find(object->m_nModelIndex) != models->end()) {
						CWorld::Remove(object);
					}
				}
				for (auto object : CPools::ms_pDummyPool) {
					if (models->find(object->m_nModelIndex) != models->end()) {
						CWorld::Remove(object);
					}
				}
			}
			/*animEntry* i = &anims[0];

			if (i->timeremain != 0) {
				of << i->timeremain << " " << i->dp.x << " " << i->dp.y << " " << i->dp.z << std::endl;
				CMatrix cmatrix(&i->frame->modelling, false);
				CVector cpos(cmatrix.pos);
				cmatrix.Rotate(i->dr.x, i->dr.y, i->dr.z);
				cmatrix.pos = cpos;
				//cmatrix.SetTranslateOnly(i->dp.x, i->dp.y, i->dp.z);


				cmatrix.UpdateRW();
				i->timeremain -= 1;
			}*/
		};
	}
	return TRUE;
}

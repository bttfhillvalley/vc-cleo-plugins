#include <filesystem>
#include "opcodes.h"
#include "../sound/sound.h"

eOpcodeResult __stdcall stopAllSounds(CScript* script)
{
	script->Collect(0);
	m_soundEngine->stopAllSounds();
	return OR_CONTINUE;
}

eOpcodeResult __stdcall stopSound(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	cleanupSound(key);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall stopSoundIndex(CScript* script)
{
	script->Collect(2);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	cleanupSound(key);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundPlaying(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	script->UpdateCompareFlag(soundMap.contains(key) && !soundMap[key].sound->isFinished());
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundStopped(CScript* script)
{
	script->Collect(1);
	string key(Params[0].cVar);
	script->UpdateCompareFlag(!(soundMap.contains(key) && !soundMap[key].sound->isFinished()));
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundPlayingIndex(CScript* script)
{
	script->Collect(2);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	script->UpdateCompareFlag(soundMap.contains(key) && !soundMap[key].sound->isFinished());
	return OR_CONTINUE;
}

eOpcodeResult __stdcall isSoundStoppedIndex(CScript* script)
{
	script->Collect(2);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	script->UpdateCompareFlag(!(soundMap.contains(key) && !soundMap[key].sound->isFinished()));
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playSound(CScript* script)
{
	script->Collect(2);
	string key(Params[0].cVar);
	playSoundFile(key, Params[0].cVar, Params[1].nVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playSoundIndex(CScript* script)
{
	script->Collect(2);
	int index = findEmptyIndex(Params[0].cVar);
	string key = getKeyIndex(Params[0].cVar, index);
	playSoundFile(key, Params[0].cVar, Params[1].nVar);
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playKeypad(CScript* script)
{
	script->Collect(1);
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%d.wav", Params[0].nVar);
	if (filesystem::exists(fullpath)) {
		m_soundEngine->play2D(fullpath);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playSoundAtLocation(CScript* script)
{
	script->Collect(6);
	string key(Params[0].cVar);
	playSoundFileLocation(key, Params[0].cVar, Params[4].nVar, Params[1].fVar, Params[2].fVar, Params[3].fVar, Params[5].fVar);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall playSoundAtLocationIndex(CScript* script)
{
	script->Collect(6);
	int index = findEmptyIndex(Params[0].cVar);
	string key = getKeyIndex(Params[0].cVar, index);
	playSoundFileLocation(key, Params[0].cVar, Params[4].nVar, Params[1].fVar, Params[2].fVar, Params[3].fVar, Params[5].fVar);
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall attachSoundToVehicle(CScript* script)
{
	script->Collect(7);
	int index = 0;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);

	if (vehicle) {
		string key = getKeyIndex(Params[1].cVar, Params[0].nVar);
		attachSoundFileToVehicle(vehicle, key, Params[1].cVar, Params[5].nVar, Params[2].fVar, Params[3].fVar, Params[4].fVar, Params[6].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setFrequency(CScript* script)
{
	script->Collect(3);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	if (soundMap.contains(key)) {
		soundMap[key].sound->setPlaybackSpeed(Params[2].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setVolume(CScript* script)
{
	script->Collect(3);
	string key = getKeyIndex(Params[0].cVar, Params[1].nVar);
	if (soundMap.contains(key)) {
		soundMap[key].sound->setVolume(Params[2].fVar);
	}
	return OR_CONTINUE;
}

eOpcodeResult __stdcall attachSoundToVehicleIndex(CScript* script)
{
	script->Collect(7);
	int index = 0;
	CVehicle* vehicle = CPools::GetVehicle(Params[0].nVar);
	if (vehicle) {
		index = findEmptyIndex(Params[1].cVar);
		string key = getKeyIndex(Params[1].cVar, index);
		attachSoundFileToVehicle(vehicle, key, Params[1].cVar, Params[5].nVar, Params[2].fVar, Params[3].fVar, Params[4].fVar, Params[6].fVar);
	}
	Params[0].nVar = index;
	script->Store(1);
	return OR_CONTINUE;
}

eOpcodeResult __stdcall setDoppler(CScript* script)
{
	script->Collect(2);
	m_soundEngine->setDopplerEffectParameters(Params[0].fVar, Params[1].fVar);
	return OR_CONTINUE;
}
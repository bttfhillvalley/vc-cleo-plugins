#include "plugin.h"
#include "extensions\ScriptCommands.h"
#include "sound.h"
#include <filesystem>

ISoundEngine* m_soundEngine;
map<string, GameSound> soundMap;
bool loadedSound = false;
char volume = 0;

void cleanupSound(string key) {
	if (soundMap.contains(key)) {
		soundMap[key].sound->stop();
		soundMap[key].sound->drop();
		soundMap.erase(key);
	}
}

string getKeyIndex(char* name, int index) {
	string key(name);
	return key + "_" + to_string(index);
}

int findEmptyIndex(char* name) {
	string key;
	int index = 0;
	do {
		key = getKeyIndex(name, ++index);
	} while (soundMap.contains(key));
	return index;
}

void playSoundFile(string key, char* filename, bool loop) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", filename);
	if (!filesystem::exists(fullpath)) {
		return;
	}
	cleanupSound(key);
	soundMap[key].sound = m_soundEngine->play2D(fullpath, loop, false, true, ESM_AUTO_DETECT, true);
	soundMap[key].spatial = false;
}

void playSoundFileLocation(string key, char* filename, bool loop, float x, float y, float z, float minDistance) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", filename);
	if (!filesystem::exists(fullpath)) {
		return;
	}
	cleanupSound(key);
	vec3df pos;
	pos.X = x;
	pos.Y = -1.0f * y;
	pos.Z = z;
	soundMap[key].offset = CVector(pos.X, pos.Y, pos.Z);
	soundMap[key].sound = m_soundEngine->play3D(fullpath, pos, loop, false, true, ESM_AUTO_DETECT, true);
	soundMap[key].sound->setMinDistance(minDistance);
	soundMap[key].spatial = true;
}

void attachSoundFileToVehicle(CVehicle* vehicle, string key, char* filename, bool loop, float x, float y, float z, float minDistance) {
	char fullpath[128];
	snprintf(fullpath, 128, ".\\sound\\%s", filename);
	if (!filesystem::exists(fullpath)) {
		return;
	}
	cleanupSound(key);
	vec3df pos;
	soundMap[key].vehicle = vehicle;
	soundMap[key].offset = CVector(x, y, z);
	Command<Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(vehicle, soundMap[key].offset.x, soundMap[key].offset.y, soundMap[key].offset.z, &pos.X, &pos.Y, &pos.Z);
	pos.Y *= -1.0;

	soundMap[key].sound = m_soundEngine->play3D(fullpath, pos, loop, false, true, ESM_AUTO_DETECT, true);
	soundMap[key].sound->setMinDistance(minDistance);
	soundMap[key].spatial = true;
}
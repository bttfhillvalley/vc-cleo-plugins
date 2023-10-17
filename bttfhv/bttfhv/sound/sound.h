#pragma once
#include <map>
#include "plugin.h"
#include "irrKlang.h"

using namespace std;
using namespace irrklang;
using namespace plugin;

struct GameSound {
	ISound* sound;
	CVehicle* vehicle;
	CVector offset{ 0.0,0.0,0.0 };
	bool spatial;
};

extern ISoundEngine* m_soundEngine;
extern bool loadedSound;
extern char volume;
extern map<string, GameSound> soundMap;

void cleanupSound(string key);
string getKeyIndex(char* name, int index);
int findEmptyIndex(char* name);
void playSoundFile(string key, char* filename, bool looped);
void playSoundFileLocation(string key, char* filename, bool looped, float x, float y, float z, float minDistance);
void attachSoundFileToVehicle(CVehicle* vehicle, string key, char* filename, bool looped, float x, float y, float z, float minDistance);
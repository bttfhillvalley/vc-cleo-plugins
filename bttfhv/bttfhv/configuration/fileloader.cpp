#include <fstream>
#include "CModelInfo.h"
#include "CFileMgr.h"

#include "../utils/math.h"
#include "config.h"
#include "fileloader.h"

using namespace std;

unsigned char work_buff[55000];

map<string, tHandlingData*> handlingData;
map<string, tFlyingHandlingData*> flyingHandlingData;
map<string, tBoatHandlingData*> boatHandlingData;
map<string, tBikeHandlingData*> bikeHandlingData;
map<string, int> keyMap;

void ConvertBikeDataToGameUnits(tBikeHandlingData* handling)
{
	handling->m_fMaxLean = sin(radians(handling->m_fMaxLean));
	handling->m_fFullAnimLean = radians(handling->m_fFullAnimLean);
	handling->m_fWheelieAng = sin(radians(handling->m_fWheelieAng));
	handling->m_fStoppieAng = sin(radians(handling->m_fStoppieAng));
}

bool doesFileExist(const char* filepath) {
	fstream infile(filepath);
	return infile.good();
}

void InitializeKeyMap(void) {
	// Alphabet
	keyMap.clear();

	for (char c = 'A'; c <= 'Z'; c++) {
		keyMap[string(1, c)] = MapVirtualKey(c, MAPVK_VK_TO_VSC);
	}

	// Number keys
	for (char c = '0'; c <= '9'; c++) {
		keyMap[string(1, c)] = MapVirtualKey(c, MAPVK_VK_TO_VSC);
	}

	// Numpad
	for (int i = 0; i <= 9; i++) {
		keyMap["NUM" + to_string(i)] = VK_NUMPAD0 + i;
	}

	// Function keys
	for (int i = 1; i <= 12; i++) {
		keyMap["F" + to_string(i)] = VK_F1 + i - 1;
	}

	// Special keys
	keyMap["BACKSPACE"] = VK_BACK;
	keyMap["TAB"] = VK_TAB;
	keyMap["RETURN"] = VK_RETURN;

	keyMap["PAUSE"] = VK_PAUSE;
	keyMap["CAPSLOCK"] = VK_CAPITAL;
	keyMap["SPACE"] = VK_SPACE;
	keyMap["PGUP"] = VK_PRIOR;
	keyMap["PGDN"] = VK_NEXT;
	keyMap["END"] = VK_END;
	keyMap["HOME"] = VK_HOME;
	keyMap["LEFT"] = VK_LEFT;
	keyMap["UP"] = VK_UP;
	keyMap["RIGHT"] = VK_RIGHT;
	keyMap["DOWN"] = VK_DOWN;
	keyMap["INS"] = VK_INSERT;
	keyMap["DEL"] = VK_DELETE;
	keyMap["NUM *"] = VK_MULTIPLY;
	keyMap["NUM +"] = VK_ADD;
	keyMap["NUM ."] = VK_SEPARATOR;
	keyMap["NUM -"] = VK_SUBTRACT;
	keyMap["NUM /"] = VK_DIVIDE;
	keyMap["NUM ENT"] = VK_RETURN & 0x8000;
	keyMap["NUMLOCK"] = VK_NUMLOCK;
	keyMap["SCROLL LOCK"] = VK_SCROLL;
	keyMap["LSHIFT"] = VK_LSHIFT;
	keyMap["RSHIFT"] = VK_RSHIFT;
	keyMap["LCTRL"] = VK_LCONTROL;
	keyMap["RCTRL"] = VK_RCONTROL;
	keyMap["LALT"] = VK_MENU;
	keyMap["RALT"] = VK_RMENU;
	keyMap[";"] = VK_OEM_1;
	keyMap["+"] = VK_OEM_PLUS;
	keyMap[","] = VK_OEM_COMMA;
	keyMap["-"] = VK_OEM_MINUS;
	keyMap["."] = VK_OEM_PERIOD;
	keyMap["/"] = VK_OEM_2;
	keyMap["`"] = VK_OEM_3;
	keyMap["["] = VK_OEM_4;
	keyMap["\\"] = VK_OEM_5;
	keyMap["]"] = VK_OEM_6;
	keyMap["'"] = VK_OEM_7;
}

void LoadKeyConfig(void) {
	int fd;
	char line[256];
	int start, end;
	char key[64];
	char vk[64];
	if (!doesFileExist(".\\BTTFHV.CFG")) {
		return;
	}
	CFileMgr::SetDir("");
	fd = CFileMgr::OpenFile("BTTFHV.CFG", "r");

	while (CFileMgr::ReadLine(fd, line, sizeof(line))) {
		for (start = 0; ; start++)
			if (line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
				break;
		// find end of line
		for (end = start; ; end++) {
			if (line[end] == '\0' || line[end] == '\n')
				break;
			if (line[end] == '=' || line[end] == '\r')
				line[end] = ' ';
		}
		line[end] = '\0';

		// empty line
		if (line[start] == '#' || line[start] == '\0')
			continue;

		sscanf(line, "%s %s", key, vk);
		configKeys[string(key)] = keyMap[string(vk)];
	}
}

void LoadAdditionalHandlingData(void)
{
	char* start, * end;
	char line[201];	// weird value
	char delim[4];	// not sure
	char* word;
	string key;
	int field;
	int keepGoing;
	tHandlingData* handling;
	tFlyingHandlingData* flyingHandling;
	tBoatHandlingData* boatHandling;
	tBikeHandlingData* bikeHandling;

	if (!doesFileExist(".\\DATA\\HANDLING_ADDITIONAL.CFG")) {
		return;
	}

	CFileMgr::SetDir("DATA");
	CFileMgr::LoadFile("HANDLING_ADDITIONAL.CFG", work_buff, sizeof(work_buff), "r");
	CFileMgr::SetDir("");

	start = (char*)work_buff;
	end = start + 1;
	keepGoing = 1;

	while (keepGoing) {
		// find end of line
		while (*end != '\n') end++;

		// get line
		strncpy(line, start, end - start);
		line[end - start] = '\0';
		start = end + 1;
		end = start + 1;

		// yeah, this is kinda crappy
		if (strcmp(line, ";the end") == 0)
			keepGoing = 0;
		else if (line[0] != ';') {
			if (line[0] == '!') {
				// Bike data
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0: break;
					case  1:
						key = string(word);
						bikeHandling = new tBikeHandlingData();
						bikeHandlingData[key] = bikeHandling;
						break;
					case  2: bikeHandling->m_fLeanFwdCOM = (float)atof(word); break;
					case  3: bikeHandling->m_fLeanFwdForce = (float)atof(word); break;
					case  4: bikeHandling->m_fLeanBakCOM = (float)atof(word); break;
					case  5: bikeHandling->m_fLeanBakForce = (float)atof(word); break;
					case  6: bikeHandling->m_fMaxLean = (float)atof(word); break;
					case  7: bikeHandling->m_fFullAnimLean = (float)atof(word); break;
					case  8: bikeHandling->m_fDesLean = (float)atof(word); break;
					case  9: bikeHandling->m_fSpeedSteer = (float)atof(word); break;
					case 10: bikeHandling->m_fSlipSteer = (float)atof(word); break;
					case 11: bikeHandling->m_fNoPlayerCOMz = (float)atof(word); break;
					case 12: bikeHandling->m_fWheelieAng = (float)atof(word); break;
					case 13: bikeHandling->m_fStoppieAng = (float)atof(word); break;
					case 14: bikeHandling->m_fWheelieSteer = (float)atof(word); break;
					case 15: bikeHandling->m_fWheelieStabMult = (float)atof(word); break;
					case 16: bikeHandling->m_fStoppieStabMult = (float)atof(word); break;
					}
					field++;
				}
				ConvertBikeDataToGameUnits(bikeHandling);
			}
			else if (line[0] == '$') {
				// Flying data
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0: break;
					case  1:
						key = string(word);
						flyingHandling = new tFlyingHandlingData();
						flyingHandlingData[key] = flyingHandling;
						break;
					case  2: flyingHandling->fThrust = (float)atof(word); break;
					case  3: flyingHandling->fThrustFallOff = (float)atof(word); break;
					case  4: flyingHandling->fYaw = (float)atof(word); break;
					case  5: flyingHandling->fYawStab = (float)atof(word); break;
					case  6: flyingHandling->fSideSlip = (float)atof(word); break;
					case  7: flyingHandling->fRoll = (float)atof(word); break;
					case  8: flyingHandling->fRollStab = (float)atof(word); break;
					case  9: flyingHandling->fPitch = (float)atof(word); break;
					case 10: flyingHandling->fPitchStab = (float)atof(word); break;
					case 11: flyingHandling->fFormLift = (float)atof(word); break;
					case 12: flyingHandling->fAttackLift = (float)atof(word); break;
					case 13: flyingHandling->fMoveRes = (float)atof(word); break;
					case 14: flyingHandling->vecTurnRes.x = (float)atof(word); break;
					case 15: flyingHandling->vecTurnRes.y = (float)atof(word); break;
					case 16: flyingHandling->vecTurnRes.z = (float)atof(word); break;
					case 17: flyingHandling->vecSpeedRes.x = (float)atof(word); break;
					case 18: flyingHandling->vecSpeedRes.y = (float)atof(word); break;
					case 19: flyingHandling->vecSpeedRes.z = (float)atof(word); break;
					}
					field++;
				}
			}
			else if (line[0] == '%') {
				// Boat data
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0: break;
					case  1:
						key = string(word);
						boatHandling = new tBoatHandlingData();
						boatHandlingData[key] = boatHandling;
						break;
					case  2: boatHandling->m_fThrustY = (float)atof(word); break;
					case  3: boatHandling->m_fThrustZ = (float)atof(word); break;
					case  4: boatHandling->m_fThrustAppZ = (float)atof(word); break;
					case  5: boatHandling->m_fAqPlaneForce = (float)atof(word); break;
					case  6: boatHandling->m_fAqPlaneLimit = (float)atof(word); break;
					case  7: boatHandling->m_fAqPlaneOffset = (float)atof(word); break;
					case  8: boatHandling->m_fWaveAudioMult = (float)atof(word); break;
					case  9: boatHandling->m_vMoveRes.x = (float)atof(word); break;
					case 10: boatHandling->m_vMoveRes.y = (float)atof(word); break;
					case 11: boatHandling->m_vMoveRes.z = (float)atof(word); break;
					case 12: boatHandling->m_vTurnRes.x = (float)atof(word); break;
					case 13: boatHandling->m_vTurnRes.y = (float)atof(word); break;
					case 14: boatHandling->m_vTurnRes.z = (float)atof(word); break;
					case 15: boatHandling->m_fLookLRBehindCamHeight = (float)atof(word); break;
					}
					field++;
				}
			}
			else {
				field = 0;
				strcpy(delim, " \t");
				// FIX: game seems to use a do-while loop here
				for (word = strtok(line, delim); word; word = strtok(NULL, delim)) {
					switch (field) {
					case  0:
						key = string(word);
						handling = new tHandlingData();
						handlingData[key] = handling;
						break;
					case  1: handling->fMass = (float)atof(word); break;
					case  2: handling->m_vDimensions.x = (float)atof(word); break;
					case  3: handling->m_vDimensions.y = (float)atof(word); break;
					case  4: handling->m_vDimensions.z = (float)atof(word); break;
					case  5: handling->m_vecCentreOfMass.x = (float)atof(word); break;
					case  6: handling->m_vecCentreOfMass.y = (float)atof(word); break;
					case  7: handling->m_vecCentreOfMass.z = (float)atof(word); break;
					case  8: handling->nPercentSubmerged = atoi(word); break;
					case  9: handling->fTractionMultiplier = (float)atof(word); break;
					case 10: handling->fTractionLoss = (float)atof(word); break;
					case 11: handling->fTractionBias = (float)atof(word); break;
					case 12: handling->m_transmissionData.m_nNumberOfGears = atoi(word); break;
					case 13: handling->m_transmissionData.m_fMaxGearVelocity = (float)atof(word); break;
					case 14: handling->m_transmissionData.m_fEngineAcceleration = (float)atof(word) * 0.4f; break;
					case 15: handling->m_transmissionData.m_nDriveType = word[0]; break;
					case 16: handling->m_transmissionData.m_nEngineType = word[0]; break;
					case 17: handling->fBrakeDeceleration = (float)atof(word); break;
					case 18: handling->fBrakeBias = (float)atof(word); break;
					case 19: handling->bABS = !!atoi(word); break;
					case 20: handling->fSteeringLock = (float)atof(word); break;
					case 21: handling->fSuspensionForceLevel = (float)atof(word); break;
					case 22: handling->fSuspensionDampingLevel = (float)atof(word); break;
					case 23: handling->fSeatOffsetDistance = (float)atof(word); break;
					case 24: handling->fCollisionDamageMultiplier = (float)atof(word); break;
					case 25: handling->nMonetaryValue = atoi(word); break;
					case 26: handling->fSuspUpperLimit = (float)atof(word); break;
					case 27: handling->fSuspLowerLimit = (float)atof(word); break;
					case 28: handling->fSuspBias = (float)atof(word); break;
					case 29: handling->fSuspAntiDiveMultiplier = (float)atof(word); break;
					case 30:
						sscanf(word, "%x", &handling->uFlags);
						handling->m_transmissionData.m_nHandlingFlags = handling->uFlags;
						break;
					case 31: handling->bFrontLights = eVehicleLightsSize(atoi(word)); break;
					case 32: handling->bRearLights = eVehicleLightsSize(atoi(word)); break;
					}
					field++;
				}
				gHandlingDataMgr.ConvertDataToGameUnits(handling);
			}
		}
	}
}

void LoadAdditionalVehicleColours(void)
{
	int fd;
	int i;
	char line[1024];
	int start, end;
	int section, numCols;
	enum {
		NONE,
		COLOURS,
		CARS
	};
	int r, g, b;
	char name[64];
	int colors[16];
	int n;

	if (!doesFileExist(".\\DATA\\CARCOLS_ADDITIONAL.DAT")) {
		return;
	}

	CFileMgr::SetDir("DATA");
	fd = CFileMgr::OpenFile("CARCOLS_ADDITIONAL.DAT", "r");
	CFileMgr::SetDir("");

	section = 0;
	numCols = 0;
	while (numCols < 256) {
		if (CVehicleModelInfo::ms_colourTextureTable[numCols].a == 0) {
			break;
		}
		numCols++;
	}
	while (CFileMgr::ReadLine(fd, line, sizeof(line))) {
		// find first valid character in line
		for (start = 0; ; start++)
			if (line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
				break;
		// find end of line
		for (end = start; ; end++) {
			if (line[end] == '\0' || line[end] == '\n')
				break;
			if (line[end] == ',' || line[end] == '\r')
				line[end] = ' ';
		}
		line[end] = '\0';

		// empty line
		if (line[start] == '#' || line[start] == '\0')
			continue;

		if (section == NONE) {
			if (line[start] == 'c' && line[start + 1] == 'o' && line[start + 2] == 'l')
				section = COLOURS;
			if (line[start] == 'c' && line[start + 1] == 'a' && line[start + 2] == 'r')
				section = CARS;
		}
		else if (line[start] == 'e' && line[start + 1] == 'n' && line[start + 2] == 'd') {
			section = NONE;
		}
		else if (section == COLOURS) {
			sscanf(&line[start],	// BUG: games doesn't add start
				"%d %d %d", &r, &g, &b);

			CVehicleModelInfo::ms_colourTextureTable[numCols].r = r;
			CVehicleModelInfo::ms_colourTextureTable[numCols].g = g;
			CVehicleModelInfo::ms_colourTextureTable[numCols].b = b;
			CVehicleModelInfo::ms_colourTextureTable[numCols].a = 0xFF;
			numCols++;
		}
		else if (section == CARS) {
			n = sscanf(&line[start],	// BUG: games doesn't add start
				"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				name,
				&colors[0], &colors[1],
				&colors[2], &colors[3],
				&colors[4], &colors[5],
				&colors[6], &colors[7],
				&colors[8], &colors[9],
				&colors[10], &colors[11],
				&colors[12], &colors[13],
				&colors[14], &colors[15]);
			CVehicleModelInfo* mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(name, NULL);
			assert(mi);
			mi->m_nNumColorVariations = (n - 1) / 2;
			for (i = 0; i < mi->m_nNumColorVariations; i++) {
				mi->m_anPrimaryColors[i] = colors[i * 2 + 0];
				mi->m_anSecondaryColors[i] = colors[i * 2 + 1];
			}
		}
	}

	CFileMgr::CloseFile(fd);
}
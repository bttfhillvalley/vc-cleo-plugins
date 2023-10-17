#pragma once
#include "plugin.h"

void setVisibility(CEntity* model, const char* component, int visible);
int getVisibility(CEntity* model, const char* component);
void moveComponent(CEntity* model, const char* component, float x, float y, float z);
void rotateComponent(CEntity* model, const char* component, float rx, float ry, float rz);
void setColor(CVehicle* vehicle, const char* component, int red, int green, int blue);
void setAlpha(CVehicle* vehicle, const char* component, int alpha);
RwUInt8 getAlpha(CVehicle* vehicle, const char* component);
void fadeAlpha(CVehicle* vehicle, const char* component, int target, int fade);
void setGlow(CVehicle* vehicle, const char* component, int glow);
int getCurrentDigit(CVehicle* vehicle, const char* component);
void digitOff(CVehicle* vehicle, const char* component);
void digitOn(CVehicle* vehicle, const char* component, int digit);
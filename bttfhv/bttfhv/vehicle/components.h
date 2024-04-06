#pragma once
#include "plugin.h"

using namespace std;
string getComponentIndex(string name, int index);
void setVisibility(CEntity* model, string component, int visible);
int getVisibility(CEntity* model, string component);
void moveComponent(CEntity* model, string component, float x, float y, float z);
inline void moveComponent(CEntity* model, string component, CVector v) { moveComponent(model, component, v.x, v.y, v.z); }
void rotateComponent(CEntity* model, string component, float rx, float ry, float rz);
inline void rotateComponent(CEntity* model, string component, CVector v) { rotateComponent(model, component, v.x, v.y, v.z); }
void setVehicleComponentFlags(CVehicle* vehicle, string component, unsigned int flags);
CVector getComponentRotation(CEntity* model, string component);
void setColor(CVehicle* vehicle, string component, int red, int green, int blue);
void setAlpha(CVehicle* vehicle, string component, int alpha);
RwUInt8 getAlpha(CVehicle* vehicle, string component);
void fadeAlpha(CVehicle* vehicle, string component, int target, int fade);
void setGlow(CVehicle* vehicle, string component, int glow);
int getCurrentDigit(CVehicle* vehicle, string component);
void digitOff(CVehicle* vehicle, string component);
void digitOn(CVehicle* vehicle, string component, int digit);
unsigned char getWheelStatusAll(CVehicle* vehicle);
CObject* createCarComponent(CVehicle* vehicle, string component);

void SetGlowAndHideIndex(CVehicle* vehicle, string component, int index);
#define _USE_MATH_DEFINES
#include <cmath>

#include "math.h"

float radians(float degrees) {
	return (float)(degrees * M_PI / 180.0);
}

float degrees(float radians) {
	return (float)(radians * 180.0 / M_PI);
}

int convertMatrixToInt(CVector vector) {
	return ((int)((vector.x + 4.0) * 100) * 1000000) + ((int)((vector.y + 4.0) * 100) * 1000) + ((int)((vector.z + 4.0) * 100));
}

void convertIntToMatrix(CVector& vector, int value) {
	int x, y, z;
	x = value / 1000000;
	y = (value - (x * 1000000)) / 1000;
	z = value - (x * 1000000) - (y * 1000);
	vector.x = x / 100.f - 4.0f;
	vector.y = y / 100.f - 4.0f;
	vector.z = z / 100.f - 4.0f;
}
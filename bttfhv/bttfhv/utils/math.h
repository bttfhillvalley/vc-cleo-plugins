#pragma once
#include "plugin.h"

float radians(float degrees);
float degrees(float radians);
int convertMatrixToInt(CVector vector);
void convertIntToMatrix(CVector& vector, int value);

inline float DotProduct(const CVector& v1, const CVector& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
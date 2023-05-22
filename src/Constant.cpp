#include "Constant.h"

// Point Mass Component
int samplerAmount = 70;	// Generate PointMass[N][N]
float length = 5.0f;

// Constant Value
float PI = 3.141592f;
float g = 9.81f;
float kx = 1.0f;
float ky = 1.0f;
float m = 10.0f;
float TimeStep = 1.0f / 90.0f;

void ResetConstant()
{
	PI = 3.141592f;
	g = 9.81f;
	kx = 1.0f;
	ky = 1.0f;
	m = 10.0f;
	TimeStep = 1.0f / 90.0f;
}
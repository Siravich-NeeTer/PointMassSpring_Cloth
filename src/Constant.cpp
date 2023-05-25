#include "Constant.h"

// Point Mass Component
int samplerAmount = 70;	// Generate PointMass[N][N]
float length = 5.0f;

// Constant Value
float PI = 3.141592f;
float g = 9.81f;
float kx = 0.9206f;
float ky = 0.44936f;
float m = 0.1572f;
float TimeStep = 1.0f / 60.0f;

void ResetConstant()
{
	PI = 3.141592f;
	g = 9.81f;
	kx = 0.9206f;
	ky = 0.44936f;
	m = 0.1572f;
	TimeStep = 1.0f / 60.0f;
}
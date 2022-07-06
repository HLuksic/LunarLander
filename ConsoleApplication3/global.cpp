#include "global.h"

float scale = 0.5f;
bool paused = false;

float RandFloat(float a, float b)
{
	return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}
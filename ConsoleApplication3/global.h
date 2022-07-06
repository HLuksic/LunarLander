#pragma once

#include "olcPixelGameEngine.h"

extern constexpr uint16_t SCREEN_WIDTH  = 550;
extern constexpr uint16_t SCREEN_HEIGHT = 390;
extern constexpr float    PLAYER_X      = (float)SCREEN_WIDTH / 2;
extern constexpr float    PLAYER_Y      = (float)SCREEN_HEIGHT / 2;
extern constexpr float    PI            = 3.14159f;
extern constexpr float    HALF_PI       = PI / 2;
extern float              scale;
extern bool               paused;

float RandFloat(float a, float b);
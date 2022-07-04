#pragma once

#include "olcPixelGameEngine.h"

extern constexpr uint16_t screenWidth  = 550;
extern constexpr uint16_t screenHeight = 390;
extern constexpr float    playerX      = (float)screenWidth / 2;
extern constexpr float    playerY      = (float)screenHeight / 2;
extern constexpr float    PI           = 3.14159f;
extern constexpr float    HALFPI       = PI / 2;
extern float              scale;
extern bool               paused;
extern bool               titleScreen;

float RandFloat(float a, float b);
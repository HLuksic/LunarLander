#pragma once

#include "olcPixelGameEngine.h"

constexpr int screenWidth = 550;
constexpr int screenHeight = 390;
constexpr float playerX = (float)screenWidth / 2;
constexpr float playerY = (float)screenWidth / 2;
constexpr float PI = 3.14159f;
constexpr float HALFPI = PI / 2;

olc::vf2d playerPos = { playerX, playerY };
olc::vf2d adjustedPos = playerPos / 2;

float scale;
bool paused;
bool titleScreen;

float RandFloat(float a, float b)
{
	return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}
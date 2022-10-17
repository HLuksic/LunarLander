#pragma once

#include <random>

extern inline constexpr uint16_t SCREEN_WIDTH  = 550;
extern inline constexpr uint16_t SCREEN_HEIGHT = 390;
extern inline constexpr float    PI            = 3.141592f;
extern inline constexpr float    HALF_PI       = PI / 2;
extern inline constexpr float    PLAYER_X      = SCREEN_WIDTH / 2;
extern inline constexpr float    PLAYER_Y      = SCREEN_HEIGHT / 2;
extern inline float				 Scale         = 0.5f;
extern inline bool				 Paused        = false;

inline float RandInRange(float a, float b) { return ((b - a) * ((float)rand() / RAND_MAX)) + a; }

constexpr float DegToRad(float a) { return a * (PI / 180.0f); }
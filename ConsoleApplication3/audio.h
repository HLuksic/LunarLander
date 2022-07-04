#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_Sound.h"

class Player;

class Audio
{
public:
	Audio();

private:
	int soundSamples[13];
public:
	bool soundPlayed;

public:
	void Play(olc::PixelGameEngine* pge, Player* player);
	void PlaySoundSample(olc::PixelGameEngine* pge, uint8_t a, uint8_t b);
};
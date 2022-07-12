#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_Sound.h"

class Player;

class Audio
{
public:
	Audio();

public:
	bool soundPlayed;
private:
	int soundSamples[13];

public:
	void Play(olc::PixelGameEngine* pge, Player* _Player);
	void PlaySampleOnce(olc::PixelGameEngine* pge, uint8_t a, uint8_t b);
};
#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_Sound.h"

class Player;

class Audio
{
public:
	Audio();

private:
	int soundSamples[12];
public:
	bool soundPlayed;

public:
	void Play(olc::PixelGameEngine* pge, Player* player);
	void PlayLandingSound(olc::PixelGameEngine* pge);
	void PlayDeathSound(olc::PixelGameEngine* pge);
	void PlayLaunchSound(olc::PixelGameEngine* pge);
};
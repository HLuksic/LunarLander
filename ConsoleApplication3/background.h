#pragma once

#include "olcPixelGameEngine.h"

class Player;

struct sStar 
{
	olc::vf2d position;
	uint8_t lightIntensity;
};

class Background
{
public:
	Background();

public:
	std::unique_ptr<olc::Sprite> sprEarth;
	std::unique_ptr<olc::Decal>	 decEarth;
private:
	std::vector<sStar> vecStars;
	olc::vf2d          earthPos;

public:
	void Draw(olc::PixelGameEngine* pge, Player* _Player);
	void Reset();
};
#pragma once

#include "olcPixelGameEngine.h"

class Player;

struct sStar 
{
	olc::vf2d position;
	float lightIntensity;
};

class Background
{
public:
	Background();

public:
	std::unique_ptr<olc::Sprite> sprEarth;
	std::unique_ptr<olc::Decal>	 decEarth;
private:
	olc::vf2d          earthPos;
	std::vector<sStar> vecStars;

public:
	void Draw(olc::PixelGameEngine* pge, Player* _Player);
	void Reset();
};
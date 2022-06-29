#pragma once

#include "olcPixelGameEngine.h"

class Player;

struct sStar 
{
	olc::vf2d	position;
	float		lightIntensity;
};

class Background
{
public:
	Background();

private:
	olc::vf2d earthPos;
	std::vector<sStar> vecStars;
	std::unique_ptr<olc::Sprite> sprEarth;

public:
	void Draw(olc::PixelGameEngine* pge, Player* player);
	void Reset();
};
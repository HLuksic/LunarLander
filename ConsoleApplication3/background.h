#pragma once

#include "olcPixelGameEngine.h"

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
	olc::vf2d						earthPos;
	std::unique_ptr<olc::Sprite>	sprEarth;
	std::vector<sStar>				vecStars;

public:
	void Draw(olc::PixelGameEngine* pge);
	static void Reset();
};
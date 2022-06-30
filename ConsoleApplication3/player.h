#pragma once

#include "olcPixelGameEngine.h"
#include "terrain.h"
#include "background.h"

class Interface;
class FileHandler;

class Player
{
public:
	Player();

public:
	int	landings;
	int	normalizedHorizontalVelocity;
	int	normalizedVerticalVelocity;
	float	angle;
	float	altitude;
	float	thrust;
	float	score;
	float	fuel;
	float	currentSegmentAngle;
	bool	dead;
	
	olc::vf2d position;
	olc::vf2d adjustedPosition;
	olc::vf2d velocity;

	std::unique_ptr<olc::Decal> decPlayer;

private:
	std::unique_ptr<olc::Sprite> sprPlayer;
	std::unique_ptr<olc::Sprite> sprPlayerLightDamage;
	std::unique_ptr<olc::Sprite> sprPlayerMediumDamage;
	std::unique_ptr<olc::Sprite> sprPlayerHeavyDamage;
	std::unique_ptr<olc::Sprite> sprPlayerDestroyed;
	std::unique_ptr<olc::Sprite> sprBurner;
	std::unique_ptr<olc::Sprite> sprEnd;

	std::unique_ptr<olc::Decal> decPlayerLightDamage;
	std::unique_ptr<olc::Decal> decPlayerMediumDamage;
	std::unique_ptr<olc::Decal> decPlayerHeavyDamage;
	std::unique_ptr<olc::Decal> decPlayerDestroyed;
	std::unique_ptr<olc::Decal> decBurner;
	std::unique_ptr<olc::Decal> decEnd;

public:
	void LandingHandler(
		olc::PixelGameEngine* pge,
		sSegment& segment, 
		Background* background, 
		Terrain* terrain, 
		Interface* userInterface,
		FileHandler* fileHandler);
	void Draw(olc::PixelGameEngine* pge, float fElapsedTime);
	void Physics(olc::PixelGameEngine* pge, Terrain* terrain, float fElapsedTime);
	void Reset();
};

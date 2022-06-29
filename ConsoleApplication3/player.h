#pragma once

#include "olcPixelGameEngine.h"
#include "terrain.h"
#include "background.h"

class Interface;

class Player
{
public:
	Player();

public:
	int	playerLandings;
	int	normalizedHorizontalVelocity;
	int	normalizedVerticalVelocity;
	float	playerAngle;
	float	playerAltitude;
	float	playerThrust;
	float	playerScore;
	float	playerFuel;
	float	currentSegmentAngle;
	bool	playerDead;
	
	olc::vf2d playerPos;
	olc::vf2d adjustedPos;
	olc::vf2d playerVel;

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
	void Collision(
		olc::PixelGameEngine* pge,
		sSegment& segment, 
		Background* background, 
		Terrain* terrain, 
		Interface* userInterface);
	void Draw(olc::PixelGameEngine* pge, float fElapsedTime);
	void Physics(olc::PixelGameEngine* pge, Terrain* terrain, float fElapsedTime);
	void Reset();
};

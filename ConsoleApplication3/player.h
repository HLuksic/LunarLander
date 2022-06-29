#pragma once

#include "terrain.h"

class Player
{
public:
	Player();

public:
	static int		playerLandings;
	static int		normalizedHorizontalVelocity;
	static int		normalizedVerticalVelocity;
	static float	playerAngle;
	static float	playerAltitude;
	static float	playerThrust;
	static float	playerScore;
	static float	playerFuel;
	static float	currentSegmentAngle;
	static bool		playerDead;

	static olc::vf2d playerVel;

private:
	static std::unique_ptr<olc::Sprite> sprPlayer;
	static std::unique_ptr<olc::Sprite> sprPlayerLightDamage;
	static std::unique_ptr<olc::Sprite> sprPlayerMediumDamage;
	static std::unique_ptr<olc::Sprite> sprPlayerHeavyDamage;
	static std::unique_ptr<olc::Sprite> sprPlayerDestroyed;
	static std::unique_ptr<olc::Sprite> sprBurner;
	static std::unique_ptr<olc::Sprite> sprEnd;

public:
	static std::unique_ptr<olc::Decal> decPlayer;

private:
	static std::unique_ptr<olc::Decal> decPlayerLightDamage;
	static std::unique_ptr<olc::Decal> decPlayerMediumDamage;
	static std::unique_ptr<olc::Decal> decPlayerHeavyDamage;
	static std::unique_ptr<olc::Decal> decPlayerDestroyed;
	static std::unique_ptr<olc::Decal> decBurner;
	static std::unique_ptr<olc::Decal> decEnd;

public:
	void Draw(olc::PixelGameEngine* pge, float fElapsedTime);
	void Collision(olc::PixelGameEngine* pge, sSegment& segment);
	void Physics(olc::PixelGameEngine* pge, float fElapsedTime);
	static void Reset();
};
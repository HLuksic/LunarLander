#pragma once

#include "olcPixelGameEngine.h"
#include "terrain.h"
#include "background.h"

class Interface;
class FileHandler;
class Audio;

class Player
{
public:
	Player();

public:
	float	  angle;
	float	  altitude;
	float	  thrust;
	float	  score;
	float	  fuel;
	float	  currentSegmentAngle;
	bool	  dead;
	uint16_t  gainedScore;
	uint16_t  landings;
	uint16_t  normHorVel;
	uint16_t  normVerVel;
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
	std::unique_ptr<olc::Decal>  decPlayerLightDamage;
	std::unique_ptr<olc::Decal>  decPlayerMediumDamage;
	std::unique_ptr<olc::Decal>  decPlayerHeavyDamage;
	std::unique_ptr<olc::Decal>  decPlayerDestroyed;
	std::unique_ptr<olc::Decal>  decBurner;
	std::unique_ptr<olc::Decal>  decEnd;

public:
	void HandleLanding(olc::PixelGameEngine* pge, Segment& segment, Background* _Background, 
		Terrain* _Terrain, Audio* _Audio, float fElapsedTime);
	void Draw(olc::PixelGameEngine* pge, float fElapsedTime);
	void Physics(olc::PixelGameEngine* pge, Terrain* _Terrain, Audio* _Audio, float fElapsedTime);
	void Reset();
private:
	bool LandingSuccessful(Segment& segment, float segmentAngle);
	void UpdateScore(float segmentAngle, bool& statsUpdated, float fElapsedTime);
};

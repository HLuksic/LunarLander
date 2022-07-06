#pragma once

#include <deque>
#include "olcPixelGameEngine.h"

class Player;
class Interface;
class Background;
class FileHandler;
class Audio;

struct sSegment 
{
	float     angle;
	bool      visited;
	olc::vf2d leftNode;
	olc::vf2d rightNode;
};

class Terrain
{
public:
	Terrain();

public:
	std::deque<sSegment> deqSegments;
	std::deque<sSegment> deqBackgroundSegments;
private:
	std::unique_ptr<olc::Sprite> sprSurface;
	std::unique_ptr<olc::Decal>  decSurface;

public:
	void HandleCollision(
		olc::PixelGameEngine* pge, 
		Player* _Player, 
		Background* _Background, 
		Interface* _Interface, 
		FileHandler* _FileHandler,
		Audio* _Audio,
		float fElapsedTime);
	void Spawn(Player* _Player);
	void Draw(olc::PixelGameEngine* pge, Player* _Player, float fElapsedTime);
	void Reset();
private:
	template<typename T>
	void CreateNewSegment(T _Terrain, bool left, olc::vf2d distanceX, olc::vf2d distanceY);
	void SpawnInitialNodes(Player* _Player);
	float GetGroundAngle(olc::vf2d node1, olc::vf2d node2);
};
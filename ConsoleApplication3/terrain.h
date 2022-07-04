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
	void Collision(
		olc::PixelGameEngine* pge, 
		Player* player, 
		Background* background, 
		Interface* userInterface, 
		FileHandler* fileHandler,
		Audio* audio);
	void Spawn(Player* player);
	void Draw(olc::PixelGameEngine* pge, Player* player, float fElapsedTime);
	float GetGroundAngle(olc::vf2d node1, olc::vf2d node2);
	void Reset();
private:
	template<typename T>
	void CreateNewSegment(T terrain, bool left, olc::vf2d distanceX, olc::vf2d distanceY);
};
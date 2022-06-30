#pragma once

#include <deque>
#include "olcPixelGameEngine.h"

class Player;
class Interface;
class Background;
class FileHandler;

struct sSegment 
{
	float angle;
	bool visited;
	olc::vf2d leftNode;
	olc::vf2d rightNode;
};

class Terrain
{
public:
	Terrain();

public:
	std::deque<sSegment> deqSegments;
	std::deque<sSegment> deqBgSegments;

private:
	std::unique_ptr<olc::Sprite> sprSurface;
	std::unique_ptr<olc::Decal> decSurface;

private:
	template<typename T>
	void CreateNewSegment(T terrain, bool left, olc::vf2d distanceX, olc::vf2d distanceY);

public:
	void Spawn(Player* player);
	void Collision(olc::PixelGameEngine* pge, Player* player, Background* background, Interface* userInterface, FileHandler* fileHandler);
	void Draw(olc::PixelGameEngine* pge, Player* player);
	void Reset();
	float GetGroundAngle(olc::vf2d node1, olc::vf2d node2);
};
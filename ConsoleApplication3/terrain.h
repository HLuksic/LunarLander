#pragma once

#include <deque>

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
	static std::deque<sSegment>	deqSegments;
	static std::deque<sSegment>	deqBgSegments;
private:
	std::unique_ptr<olc::Sprite> sprSurface;
	std::unique_ptr<olc::Decal> decSurface;

public:
	template<typename T>
	void CreateNewSegment(T terrain, bool left, olc::vf2d distanceX, olc::vf2d distanceY);
	void Spawn();
	void Draw(olc::PixelGameEngine* pge);
	static void Reset();
	float GetGroundAngle(olc::vf2d node1, olc::vf2d node2);
};
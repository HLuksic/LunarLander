#include "olcPixelGameEngine.h"

#include "terrain.h"
#include "player.h"
#include "global.h"

Terrain::Terrain()
{
	sprSurface = std::make_unique<olc::Sprite>("../gfx/ground.png");
	decSurface = std::make_unique<olc::Decal>(sprSurface.get());

	Reset();
}

template<typename T>
void Terrain::CreateNewSegment(T terrain, bool left, olc::vf2d distanceX, olc::vf2d distanceY)
{
	// New rightNode is the previous back() segments leftNode
		// New leftNode is random with regard to previous back() segments leftNode
	if (left)
	{
		olc::vf2d newRightNode = terrain->back().leftNode;
		olc::vf2d newLeftNode = {
			terrain->back().leftNode.x - RandFloat(distanceX.x, distanceX.y),
			terrain->back().leftNode.y + RandFloat(distanceY.x, distanceY.y)
		};

		terrain->push_back({
			newLeftNode,
			newRightNode,
			GetGroundAngle(newLeftNode, newRightNode),
			false
			});
	}
	else
	{
		olc::vf2d newLeftNode = terrain->front().rightNode;
		olc::vf2d newRightNode = {
			terrain->front().rightNode.x + RandFloat(distanceX.x, distanceX.y),
			terrain->front().rightNode.y + RandFloat(distanceY.x, distanceY.y)
		};

		terrain->push_front({
			newLeftNode,
			newRightNode,
			GetGroundAngle(newLeftNode, newRightNode),
			false
			});
	}
}

void Terrain::Spawn()
{
	// Spawn first nodes
	if (!deqSegments.size() && !deqBgSegments.size())
	{
		olc::vf2d node1 = {
			playerPos.x - 20.0f,
			RandFloat(screenHeight * 1.3f, screenHeight * 1.6f) };

		olc::vf2d node2 = {
			playerPos.x + 20.0f,
			RandFloat(screenHeight * 1.3f, screenHeight * 1.6f) };

		// Solid ground
		deqSegments.push_back({
			node1,
			node2,
			GetGroundAngle(node1, node2),
			false });

		// Background
		deqBgSegments.push_back({
			{ playerPos.x - 7.0f, RandFloat(screenHeight * 0.9f, screenHeight * 1.0f) },
			{ playerPos.x + 7.0f, RandFloat(screenHeight * 0.9f, screenHeight * 1.0f) },
			0.0f,
			false });
	}

	// If the front() or back() terrain segment is near the screen border, spawn new ones
	if (deqSegments.back().rightNode.x > -250)
		CreateNewSegment(&deqSegments, true, { 30.0f, 50.0f }, { -50.0f, 50.0f });

	if (deqSegments.front().leftNode.x < screenWidth + 250)
		CreateNewSegment(&deqSegments, false, { 30.0f, 50.0f }, { -50.0f, 50.0f });

	if (deqBgSegments.back().rightNode.x > -250)
		CreateNewSegment(&deqBgSegments, true, { 15.0f, 25.0f }, { -15.0f, 15.0f });

	if (deqBgSegments.front().leftNode.x < screenWidth + 250)
		CreateNewSegment(&deqBgSegments, false, { 15.0f, 25.0f }, { -15.0f, 15.0f });
}

void Terrain::Draw(olc::PixelGameEngine* pge)
{
	// Background
	for (auto& segment : deqBgSegments)
	{
		// Find segments on screen
		if (segment.rightNode.x < screenWidth / scale - adjustedPos.x &&
			segment.leftNode.x > -screenWidth / scale + adjustedPos.x)
		{
			// Fill black
			pge->DrawWarpedDecal(
				decSurface.get(),
				{
					segment.leftNode * scale + adjustedPos,
					segment.rightNode * scale + adjustedPos,
					olc::vf2d(segment.rightNode.x, segment.rightNode.y + 100.0f) * scale + adjustedPos,
					olc::vf2d(segment.leftNode.x, segment.leftNode.y + 100.0f) * scale + adjustedPos
				},
				olc::VERY_DARK_GREY
			);
		}
	}

	// Foreground
	for (auto& segment : deqSegments)
	{
		if (segment.rightNode.x < screenWidth / scale - adjustedPos.x &&
			segment.leftNode.x > -screenWidth / scale + adjustedPos.x)
		{
			// Landable segments are WHITE, highlighted
			if (abs(segment.angle) <= 0.349f && !segment.visited)
			{
				pge->DrawWarpedDecal(
					decSurface.get(),
					{
						segment.leftNode * scale + adjustedPos,
						segment.rightNode * scale + adjustedPos,
						olc::vf2d(segment.rightNode.x, segment.rightNode.y + 500.0f) * scale + adjustedPos,
						olc::vf2d(segment.leftNode.x, segment.leftNode.y + 500.0f) * scale + adjustedPos
					},
					olc::WHITE
				);
			}
			// Non-landable segments are DARK_GREY
			else
			{
				pge->DrawWarpedDecal(
					decSurface.get(),
					{
						segment.leftNode * scale + adjustedPos,
						segment.rightNode * scale + adjustedPos,
						olc::vf2d(segment.rightNode.x, segment.rightNode.y + 500.0f) * scale + adjustedPos,
						olc::vf2d(segment.leftNode.x, segment.leftNode.y + 500.0f) * scale + adjustedPos
					},
					olc::DARK_GREY
				);
			}
		}
	}
}

void Terrain::Reset()
{
	deqSegments.clear();
	deqBgSegments.clear();
}

float Terrain::GetGroundAngle(olc::vf2d node1, olc::vf2d node2)
{
	return atan2(node2.y - node1.y, node2.x - node1.x);
}

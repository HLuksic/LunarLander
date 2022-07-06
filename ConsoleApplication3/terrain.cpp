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
void Terrain::CreateNewSegment(T _Terrain, bool left, olc::vf2d distanceX, olc::vf2d distanceY)
{
	// New rightNode is the previous back() segments leftNode
	// New leftNode is random with regard to previous back() segments leftNode
	if (left)
	{
		olc::vf2d newRightNode = _Terrain->back().leftNode;
		olc::vf2d newLeftNode = {
			_Terrain->back().leftNode.x - RandFloat(distanceX.x, distanceX.y),
			_Terrain->back().leftNode.y + RandFloat(distanceY.x, distanceY.y) };

		_Terrain->push_back({ GetGroundAngle(newLeftNode, newRightNode), false, newLeftNode, newRightNode });
	}
	else
	{
		olc::vf2d newLeftNode = _Terrain->front().rightNode;
		olc::vf2d newRightNode = {
			_Terrain->front().rightNode.x + RandFloat(distanceX.x, distanceX.y),
			_Terrain->front().rightNode.y + RandFloat(distanceY.x, distanceY.y) };

		_Terrain->push_front({ GetGroundAngle(newLeftNode, newRightNode), false, newLeftNode, newRightNode });
	}
}

void Terrain::Spawn(Player* _Player)
{
	if (!deqSegments.size() && !deqBackgroundSegments.size())
		SpawnInitialNodes(_Player);

	// If the front or back segments' outer node (outermost point of terrain) 
	// is on screen, spawn a new segment
	if (deqSegments.back().leftNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * scale))
		CreateNewSegment(&deqSegments, true, { 30.0f, 50.0f }, { -50.0f, 50.0f });

	if (deqSegments.front().rightNode.x < _Player->position.x + (float)SCREEN_WIDTH / (2 * scale))
		CreateNewSegment(&deqSegments, false, { 30.0f, 50.0f }, { -50.0f, 50.0f });

	if (deqBackgroundSegments.back().leftNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * scale))
		CreateNewSegment(&deqBackgroundSegments, true, { 15.0f, 25.0f }, { -15.0f, 15.0f });

	if (deqBackgroundSegments.front().rightNode.x < _Player->position.x + (float)SCREEN_WIDTH / (2 * scale))
		CreateNewSegment(&deqBackgroundSegments, false, { 15.0f, 25.0f }, { -15.0f, 15.0f });
}

void Terrain::Collision(
	olc::PixelGameEngine* pge, 
	Player* _Player, 
	Background* _Background, 
	Interface* _Interface, 
	FileHandler* _FileHandler,
	Audio* _Audio,
	float fElapsedTime)
{
	for (auto& segment : deqSegments)
	{
		// Get segment directly under player
		if (segment.leftNode.x < _Player->position.x && segment.rightNode.x > _Player->position.x)
		{
			// Calculate point directly under player
			_Player->currentSegmentAngle = segment.angle;
			float diffY = segment.rightNode.y - segment.leftNode.y;
			float diffX = segment.rightNode.x - segment.leftNode.x;
			float mult = (_Player->position.x - segment.leftNode.x) / diffX;

			// Altitude = Y distance to the point, + 7 for lower edge of player sprite
			_Player->altitude = ((segment.leftNode.y + diffY * mult) - (_Player->position.y + 7)) / 3;

			if (_Player->altitude < 0.7f)
			{
				_Player->LandingHandler(pge, segment, _Background, this, _Audio, fElapsedTime);
				break;
			}
		}
	}
}

void Terrain::Draw(olc::PixelGameEngine* pge, Player* _Player, float fElapsedTime)
{
	//pge->DrawString({ 30,30 }, std::to_string((int)deqSegments.size()));
	//pge->DrawString({ 30,40 }, std::to_string((int)deqBackgroundSegments.size()));

	for (auto& segment : deqBackgroundSegments)
	{
		// Terrain moves inversely to player
		if (!paused)
		{
			segment.leftNode  += _Player->velocity * -0.5f * fElapsedTime;
			segment.rightNode += _Player->velocity * -0.5f * fElapsedTime;
		}

		// Cull offscreen segments
		if (segment.leftNode.x  < _Player->position.x + (float)SCREEN_WIDTH / (2 * scale) &&
			segment.rightNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * scale))
			pge->DrawWarpedDecal(
				decSurface.get(),
				{ 
					segment.leftNode * scale  + _Player->adjustedPosition,
					segment.rightNode * scale + _Player->adjustedPosition,
					olc::vf2d(segment.rightNode.x, segment.rightNode.y + 300.0f) * scale + _Player->adjustedPosition,
					olc::vf2d(segment.leftNode.x,  segment.leftNode.y + 300.0f) * scale + _Player->adjustedPosition 
				},
				olc::VERY_DARK_GREY);
	}

	for (auto& segment : deqSegments)
	{
		if (!paused)
		{
			segment.leftNode  += _Player->velocity * -1.0f * fElapsedTime;
			segment.rightNode += _Player->velocity * -1.0f * fElapsedTime;
		}

		if (segment.leftNode.x  < _Player->position.x + (float)SCREEN_WIDTH / (2 * scale) &&
			segment.rightNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * scale))
		{
			// Landable segments are WHITE, non-landable are DARK_GREY
			if (abs(segment.angle) <= 0.349f && !segment.visited)
				pge->DrawWarpedDecal(
					decSurface.get(),
					{
						segment.leftNode  * scale + _Player->adjustedPosition,
						segment.rightNode * scale + _Player->adjustedPosition,
						olc::vf2d(segment.rightNode.x, segment.rightNode.y + 500.0f) * scale + _Player->adjustedPosition,
						olc::vf2d(segment.leftNode.x,  segment.leftNode.y + 500.0f) * scale + _Player->adjustedPosition
					},
					olc::WHITE);
			else
				pge->DrawWarpedDecal(
					decSurface.get(),
					{
						segment.leftNode  * scale + _Player->adjustedPosition,
						segment.rightNode * scale + _Player->adjustedPosition,
						olc::vf2d(segment.rightNode.x, segment.rightNode.y + 500.0f) * scale + _Player->adjustedPosition,
						olc::vf2d(segment.leftNode.x,  segment.leftNode.y + 500.0f) * scale + _Player->adjustedPosition
					},
					olc::DARK_GREY);
		}
	}
}

void Terrain::Reset()
{
	deqSegments.clear();
	deqBackgroundSegments.clear();
}

void Terrain::SpawnInitialNodes(Player* _Player)
{
	olc::vf2d node1 = { _Player->position.x - 20.0f, RandFloat(SCREEN_HEIGHT * 1.3f, SCREEN_HEIGHT * 1.6f) };
	olc::vf2d node2 = { _Player->position.x + 20.0f, RandFloat(SCREEN_HEIGHT * 1.3f, SCREEN_HEIGHT * 1.6f) };

	deqSegments.push_back({ GetGroundAngle(node1, node2), false, node1, node2 });

	deqBackgroundSegments.push_back({
		0.0f,
		false,
		{ _Player->position.x - 7.0f, RandFloat(SCREEN_HEIGHT * 0.9f, SCREEN_HEIGHT * 1.0f) },
		{ _Player->position.x + 7.0f, RandFloat(SCREEN_HEIGHT * 0.9f, SCREEN_HEIGHT * 1.0f) } });
}

float Terrain::GetGroundAngle(olc::vf2d node1, olc::vf2d node2)
{
	return atan2(node2.y - node1.y, node2.x - node1.x);
}

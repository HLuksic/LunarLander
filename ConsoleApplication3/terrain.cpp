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

	olc::vf2d newLeftNode  = left ? olc::vf2d(terrain->back().leftNode.x - RandFloat(distanceX.x, distanceX.y),
                                              terrain->back().leftNode.y + RandFloat(distanceY.x, distanceY.y)) : 
                                    terrain->front().rightNode;

	olc::vf2d newRightNode = left ? terrain->back().leftNode :
                                    olc::vf2d(terrain->front().rightNode.x + RandFloat(distanceX.x, distanceX.y),
                                              terrain->front().rightNode.y + RandFloat(distanceY.x, distanceY.y));

	left ? terrain->push_back( { false, newLeftNode, newRightNode }) :
           terrain->push_front({ false, newLeftNode, newRightNode });
}

void Terrain::Spawn(Player* _Player)
{
	if (!deqFgSegments.size() && !deqBgSegments.size())
		SpawnInitialNodes(_Player);

	// If the front or back segments' outer node (last point of terrain) 
	// is on screen, spawn a new segment
	if (deqFgSegments.back().leftNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * Scale))
		CreateNewSegment(&deqFgSegments, true, { 30.0f, 50.0f }, { -50.0f, 50.0f });

	if (deqFgSegments.front().rightNode.x < _Player->position.x + (float)SCREEN_WIDTH / (2 * Scale))
		CreateNewSegment(&deqFgSegments, false, { 30.0f, 50.0f }, { -50.0f, 50.0f });

	if (deqBgSegments.back().leftNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * Scale))
		CreateNewSegment(&deqBgSegments, true, { 15.0f, 25.0f }, { -15.0f, 15.0f });

	if (deqBgSegments.front().rightNode.x < _Player->position.x + (float)SCREEN_WIDTH / (2 * Scale))
		CreateNewSegment(&deqBgSegments, false, { 15.0f, 25.0f }, { -15.0f, 15.0f });
}

void Terrain::HandleCollision(
	olc::PixelGameEngine* pge, 
	Player* _Player, 
	Background* _Background, 
	Interface* _Interface, 
	FileHandler* _FileHandler,
	Audio* _Audio,
	float fElapsedTime)
{
	for (auto& segment : deqFgSegments)
	{
		// Get segment directly under player
		if (segment.leftNode.x < _Player->position.x && segment.rightNode.x > _Player->position.x)
		{
			// Calculate point directly under player
			_Player->currentSegmentAngle = GetGroundAngle(segment.leftNode, segment.rightNode);
			float diffY = segment.rightNode.y - segment.leftNode.y;
			float diffX = segment.rightNode.x - segment.leftNode.x;
			float mult = (_Player->position.x - segment.leftNode.x) / diffX;

			// Altitude = Y distance to the point, + 7 for lower edge of player sprite
			_Player->altitude = ((segment.leftNode.y + diffY * mult) - (_Player->position.y + 7)) / 3;

			if (_Player->altitude < 0.7f)
			{
				_Player->HandleLanding(pge, segment, _Background, this, _Audio, fElapsedTime);
				break;
			}
		}
	}
}

void Terrain::Draw(olc::PixelGameEngine* pge, Player* _Player, float fElapsedTime)
{
	// Yes, this approach slows down as the number of total segments increases,
	// but I can't think of a reasonably simple way to mitigate this. 
	// The performance impact isn't terrible anyway.
	for (auto& segment : deqBgSegments)
	{
		// Terrain moves inversely to player
		if (!Paused)
		{
			segment.leftNode  += _Player->velocity * -0.5f * fElapsedTime;
			segment.rightNode += _Player->velocity * -0.5f * fElapsedTime;
		}

		// Cull offscreen segments
		if (segment.leftNode.x  < _Player->position.x + (float)SCREEN_WIDTH / (2 * Scale) &&
			segment.rightNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * Scale))
			pge->DrawWarpedDecal(
				decSurface.get(),
				{ 
					segment.leftNode * Scale  + _Player->adjustedPosition,
					segment.rightNode * Scale + _Player->adjustedPosition,
					olc::vf2d(segment.rightNode.x, segment.rightNode.y + 300.0f) * Scale + _Player->adjustedPosition,
					olc::vf2d(segment.leftNode.x,  segment.leftNode.y + 300.0f) * Scale + _Player->adjustedPosition 
				},
				olc::VERY_DARK_GREY);
	}

	for (auto& segment : deqFgSegments)
	{
		if (!Paused)
		{
			segment.leftNode  += _Player->velocity * -1.0f * fElapsedTime;
			segment.rightNode += _Player->velocity * -1.0f * fElapsedTime;
		}

		if (segment.leftNode.x  < _Player->position.x + (float)SCREEN_WIDTH / (2 * Scale) &&
			segment.rightNode.x > _Player->position.x - (float)SCREEN_WIDTH / (2 * Scale))
		{
			// Landable segments are WHITE, non-landable are DARK_GREY
			if (abs(GetGroundAngle(segment.leftNode, segment.rightNode)) <= 0.349f && !segment.visited)
				pge->DrawWarpedDecal(
					decSurface.get(),
					{
						segment.leftNode  * Scale + _Player->adjustedPosition,
						segment.rightNode * Scale + _Player->adjustedPosition,
						olc::vf2d(segment.rightNode.x, segment.rightNode.y + 500.0f) * Scale + _Player->adjustedPosition,
						olc::vf2d(segment.leftNode.x,  segment.leftNode.y + 500.0f) * Scale + _Player->adjustedPosition
					},
					olc::WHITE);
			else
				pge->DrawWarpedDecal(
					decSurface.get(),
					{
						segment.leftNode  * Scale + _Player->adjustedPosition,
						segment.rightNode * Scale + _Player->adjustedPosition,
						olc::vf2d(segment.rightNode.x, segment.rightNode.y + 500.0f) * Scale + _Player->adjustedPosition,
						olc::vf2d(segment.leftNode.x,  segment.leftNode.y + 500.0f) * Scale + _Player->adjustedPosition
					},
					olc::DARK_GREY);
		}
	}
}

void Terrain::Reset()
{
	deqFgSegments.clear();
	deqBgSegments.clear();
}

void Terrain::SpawnInitialNodes(Player* _Player)
{
	deqFgSegments.push_back({ false, 
		{ _Player->position.x - 20.0f, RandFloat(SCREEN_HEIGHT * 1.4f, SCREEN_HEIGHT * 1.5f) },
		{ _Player->position.x + 20.0f, RandFloat(SCREEN_HEIGHT * 1.4f, SCREEN_HEIGHT * 1.5f) } });

	deqBgSegments.push_back({ false,
		{ _Player->position.x - 7.0f, RandFloat(SCREEN_HEIGHT * 0.95f, SCREEN_HEIGHT * 1.0f) },
		{ _Player->position.x + 7.0f, RandFloat(SCREEN_HEIGHT * 0.95f, SCREEN_HEIGHT * 1.0f) } });
}

float Terrain::GetGroundAngle(olc::vf2d node1, olc::vf2d node2)
{
	return atan2(node2.y - node1.y, node2.x - node1.x);
}
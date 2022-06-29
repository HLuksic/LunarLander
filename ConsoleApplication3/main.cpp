#define OLC_PGE_APPLICATION
//#define DEBUG

#include <iostream>
#include <random>
#include <ctime>
#include <deque>
#include <algorithm>
#include "olcPixelGameEngine.h"

constexpr float PI = 3.14159f;
constexpr float PI2 = PI/2;

// TODO: 
// FOREGROUND
// BLACK DECALS FOR TERRAIN
// STARS
// DEBRIS PARTICLES, 
// SOUNDS,
// HIGH SCORE, 
// ADD DIFFICULTY (choose with numpad in title)

class lunarLander : public olc::PixelGameEngine
{
public:
	lunarLander()
	{
		sAppName = "Lunar lander";
	}

private:
	int screenHeight;
	int screenWidth;
	int playerLandings;
	float playerAngle;
	float playerAltitude;
	float playerThrust;
	float playerScore;
	float playerFuel;
	bool playerDead;
	int normalizedHorizontalVelocity;
	int normalizedVerticalVelocity;
	float scale;
	float currentSegmentAngle;
	bool titleScreen;
	bool paused;

	std::unique_ptr<olc::Sprite> sprPlayer;
	std::unique_ptr<olc::Sprite> sprPlayerLightDamage;
	std::unique_ptr<olc::Sprite> sprPlayerMediumDamage;
	std::unique_ptr<olc::Sprite> sprPlayerHeavyDamage;
	std::unique_ptr<olc::Sprite> sprPlayerDestroyed;
	std::unique_ptr<olc::Sprite> sprBurnerEnd;
	std::unique_ptr<olc::Sprite> sprEnd;
	std::unique_ptr<olc::Sprite> sprEarth;
	std::unique_ptr<olc::Sprite> sprSurface;

	std::unique_ptr<olc::Decal> decPlayer;
	std::unique_ptr<olc::Decal> decPlayerLightDamage;
	std::unique_ptr<olc::Decal> decPlayerMediumDamage;
	std::unique_ptr<olc::Decal> decPlayerHeavyDamage;
	std::unique_ptr<olc::Decal> decPlayerDestroyed;
	std::unique_ptr<olc::Decal> decBurnerEnd;
	std::unique_ptr<olc::Decal> decEnd;
	std::unique_ptr<olc::Decal> decSurface;

	olc::vf2d playerPos;
	olc::vf2d playerVel;
	olc::vf2d earthPos;
	olc::vf2d adjustedPos;

	struct sSegment {
		olc::vf2d leftNode;
		olc::vf2d rightNode;
		float angle;
		bool visited;
	};

	struct sParticle {
		olc::vf2d position;
		float angle;
		float life;
	};

	struct sStar {
		olc::vf2d position;
		float lightIntensity;
	};

	std::deque<sSegment> deqSegments;
	std::deque<sSegment> deqBgSegments;
	std::deque<sSegment> deqFgSegments;
	std::vector<sSegment> vecParticles;
	std::vector<sStar> vecStars;

public:
	bool OnUserCreate() override
	{
		GameReset();

		// Sprites
		sprPlayerLightDamage = std::make_unique<olc::Sprite>("../gfx/landerLightDamage.png");
		sprPlayerMediumDamage = std::make_unique<olc::Sprite>("../gfx/landerMediumDamage.png");
		sprPlayerHeavyDamage = std::make_unique<olc::Sprite>("../gfx/landerHeavyDamage.png");
		sprPlayerDestroyed = std::make_unique<olc::Sprite>("../gfx/landerDestroyed.png");
		sprPlayer = std::make_unique<olc::Sprite>("../gfx/lander.png");
		sprBurnerEnd = std::make_unique<olc::Sprite>("../gfx/burner.png");
		sprEnd = std::make_unique<olc::Sprite>("../gfx/end.png");
		sprEarth = std::make_unique<olc::Sprite>("../gfx/earth.png");
		sprSurface = std::make_unique<olc::Sprite>("../gfx/ground.png");

		// Decals
		decPlayerLightDamage = std::make_unique<olc::Decal>(sprPlayerLightDamage.get());
		decPlayerMediumDamage = std::make_unique<olc::Decal>(sprPlayerMediumDamage.get());
		decPlayerHeavyDamage = std::make_unique<olc::Decal>(sprPlayerHeavyDamage.get());
		decPlayerDestroyed = std::make_unique<olc::Decal>(sprPlayerDestroyed.get());
		decPlayer = std::make_unique<olc::Decal>(sprPlayer.get());
		decBurnerEnd = std::make_unique<olc::Decal>(sprBurnerEnd.get());
		decEnd = std::make_unique<olc::Decal>(sprEnd.get());
		decSurface = std::make_unique<olc::Decal>(sprSurface.get());
		
		return true;
	}

	float RandFloat(float a, float b)
	{
		return ((b - a) * ((float)rand() / RAND_MAX)) + a;
	}

	float GetGroundAngle(olc::vf2d node1, olc::vf2d node2) 
	{
		return atan2(node2.y - node1.y, node2.x - node1.x);
	}

	#ifdef DEBUG
	void Debug()
	{
		// Highlight leftmost and rightmost segments
		if (!deqSegments.empty())
		{
			DrawLine(
				deqSegments.front().leftNode * scale + adjustedPos, 
				deqSegments.front().rightNode * scale + adjustedPos, 
				olc::GREEN
			);
			DrawCircle(
				deqSegments.front().rightNode * scale + adjustedPos, 
				5, 
				olc::GREEN
			);
			DrawLine(
				deqSegments.back().leftNode * scale + adjustedPos, 
				deqSegments.back().rightNode * scale + adjustedPos, 
				olc::RED
			);
			DrawCircle(
				deqSegments.back().leftNode * scale + adjustedPos, 
				5, 
				olc::RED
			);
		}
		// Highlight segment & exact point under player
		for (auto& segment : deqSegments)
		{
			if (segment.leftNode.x < playerPos.x && segment.rightNode.x > playerPos.x)
			{
				DrawCircle(segment.leftNode * scale + adjustedPos, 5, olc::YELLOW);
				DrawCircle(segment.rightNode * scale + adjustedPos, 5, olc::YELLOW);
		
				float diffy = segment.rightNode.y - segment.leftNode.y;
				float diffx = segment.rightNode.x - segment.leftNode.x;
				float mult = (playerPos.x - segment.leftNode.x) / diffx;
				olc::vf2d newpoint = { playerPos.x, segment.leftNode.y + diffy * mult };
				DrawCircle(newpoint * scale + adjustedPos, 5, olc::YELLOW);
			}
		}
		// Player collision circle
		DrawCircle(playerPos * scale + adjustedPos, int(7 * scale), olc::RED);
		// Deque sizes
		DrawString(50, 50, std::to_string(deqSegments.size()));
		DrawString(60, 60, std::to_string(deqBgSegments.size()));
	}
	#endif
	
	void GameReset() 
	{
		srand((unsigned int)time(0));

		screenHeight = ScreenHeight();
		screenWidth = ScreenWidth();

		// Player
		playerAngle = 0.0f;
		playerPos = { (float)screenWidth / 2, (float)screenHeight / 2 };
		playerVel = { 0.0f, 0.7f };
		playerThrust = 0.0f;
		playerScore = 0;
		playerFuel = 2250;
		playerLandings = 0;
		playerDead = false;
		paused = false;
		titleScreen = true;

		// Environment
		adjustedPos = playerPos / 2;
		scale = 0.5f;
		earthPos = { RandFloat(100.0f, 400.0f), RandFloat(100.0f, 150.0f) };

		deqSegments.clear();
		deqBgSegments.clear();

		for (int i = 0; i < 50; i++)
		{
			sStar star;
			star.position = olc::vf2d(RandFloat(-225.0f, 775.0f), RandFloat(-98.0f, 293.0f));
			star.lightIntensity = RandFloat(0.0f, 1.0f);

			vecStars.push_back(star);
		}
	}

	void Physics(float fElapsedTime) 
	{ 
		static float Time = 0.0f;

		// Divide by 3 for believable velocity, this is what's displayed
		normalizedHorizontalVelocity = abs((int)(playerVel.x / 3));
		normalizedVerticalVelocity = abs((int)(playerVel.y / 3));

		if (!paused) // Disable everything if paused
		{
			// Angle reset if full circle
			if (abs(playerAngle) > 6.283f) playerAngle = 0.0f;

			// Directional velocity
			playerVel += { 
				playerThrust * cos(playerAngle + PI * 0.5f) * fElapsedTime, 
				playerThrust * sin(playerAngle + PI * 0.5f) * fElapsedTime 
			};
		
			// Gravity
			playerVel.y += 15.0f * fElapsedTime;

			// +1 score per second, this is why score is float
			playerScore += fElapsedTime;

			// Player is stationary, terrain moves inversely
			for (auto& segment : deqSegments)
			{
				segment.leftNode += playerVel * -1.0f * fElapsedTime;
				segment.rightNode += playerVel * -1.0f * fElapsedTime;
			}

			// Background moves slower than foreground
			for (auto& segment : deqBgSegments)
			{
				segment.leftNode += playerVel * -0.3f * fElapsedTime;
				segment.rightNode += playerVel * -0.3f * fElapsedTime;
			}

			if (GetKey(olc::Key::W).bHeld && (int)playerFuel)
			{
				playerThrust = -40.0f;
				playerFuel -= 100.0f * fElapsedTime;
			}
			else 
			{
				playerThrust = 0.0f;
			}

			if (GetKey(olc::Key::A).bHeld && (int)playerFuel)
			{
				playerAngle -= 1.5f * fElapsedTime;
				playerFuel -= 10.0f * fElapsedTime;
			}
			
			if (GetKey(olc::Key::D).bHeld && (int)playerFuel)
			{
				playerAngle += 1.5f * fElapsedTime;
				playerFuel -= 10.0f * fElapsedTime;
			}
		}

		// Shift to change zoom
		if (GetKey(olc::SHIFT).bPressed)
		{
			if (scale == 1.5f)
			{
				scale = 0.5f;
				adjustedPos = playerPos * 0.5f; // Adjusted position used in Draw() so everything stays centered
			}
			else
			{
				scale = 1.5f;
				adjustedPos = playerPos * -0.5f;
			}
		}

		for (auto& segment : deqSegments)
		{
			// Get segment directly under player
			if (segment.leftNode.x < playerPos.x && segment.rightNode.x > playerPos.x)
			{
				// Calculate point directly under player
				currentSegmentAngle = segment.angle;
				float diffY = segment.rightNode.y - segment.leftNode.y;
				float diffX = segment.rightNode.x - segment.leftNode.x;
				float mult = (playerPos.x - segment.leftNode.x) / diffX;
				// olc::vf2d newpoint = { playerPos.x, segment.leftNode.y + diffy * mult };
				// Altitude = Y distance to newpoint, + 7 as lower edge of player sprite
				playerAltitude = ((segment.leftNode.y + diffY * mult) - (playerPos.y + 7)) / 3;

				if (playerAltitude < 0.7f)
				{
					LandingHandler(segment);
					break;
				}
			}
		}
	}

	void LandingHandler(sSegment& segment) 
	{
		paused = true; // Pause game while landed
		int vel = normalizedHorizontalVelocity + normalizedVerticalVelocity;
		// Successful landing
		if (normalizedHorizontalVelocity <= 3 &&
			normalizedVerticalVelocity <= 2 &&
			abs(segment.angle) <= 0.349f && // 20 degrees
			!segment.visited &&
			abs(playerAngle - segment.angle) <= 0.087f) // 5 degrees
		{
			switch (vel)
			{
				case 0:
					DrawString({ int(screenWidth * 0.39f), int(screenHeight * 0.25f) }, "PERFECT LANDING!");
					break;
				case 1:
					DrawString({ int(screenWidth * 0.4f), int(screenHeight * 0.25f) }, "Great landing!");
					break;
				case 2:
					DrawString({ int(screenWidth * 0.4f), int(screenHeight * 0.25f) }, "Good landing!");
					break;
				case 3:
					DrawString({ int(screenWidth * 0.41f), int(screenHeight * 0.25f) }, "You made it!");
					break;
				case 4:
					DrawString({ int(screenWidth * 0.39f), int(screenHeight * 0.25f) }, "Bit shaky there!");
					break;
				case 5:
					DrawString({ int(screenWidth * 0.39f), int(screenHeight * 0.25f) }, "Almost lost it!");
					break;
				default:
					break;
			}

			DrawString(
				{ int(screenWidth * 0.47f), int(screenHeight * 0.2f) }, 
				"+" + std::to_string(int(50 + abs(segment.angle) * 544 * (5 - vel)))
			);
			DrawString( { int(screenWidth * 0.435f), int(screenHeight * 0.4f) }, "+500 fuel", olc::DARK_GREY);
			DrawStringDecal( { screenWidth * 0.33f, screenHeight * 0.8f }, "Press SPACE to continue!");
			
			// Unpause game when pressing space
			if (GetKey(olc::Key::SPACE).bPressed)
			{
				// Launch player based on ground angle
				playerVel = { -cos(playerAngle + PI2) * 60.0f, -sin(playerAngle + PI2) * 60.0f };
				playerScore += int(50 + abs(segment.angle) * 544 * (5 - vel)); 
				playerFuel += 500;
				playerLandings += 1;
				segment.visited = true;
				paused = false;
			}
		}	
		else
		{
			playerDead = true;

			if (vel < 7)
				DrawString( { int(screenWidth * 0.31f), int(screenHeight * 0.25f) }, "You broke the landing gear!");

			else if (vel < 10)
				DrawString( { int(screenWidth * 0.4f), int(screenHeight * 0.25f) }, "You crashed it!");

			else if (vel < 20)
				DrawString( { int(screenWidth * 0.4f), int(screenHeight * 0.25f) }, "You wrecked it!");

			else if (vel >= 20)
				DrawString( { int(screenWidth * 0.37f), int(screenHeight * 0.25f) }, "YOU BLEW A CRATER!");

			DrawStringDecal( { screenWidth * 0.33f, screenHeight * 0.8f }, "Press SPACE to restart!");
			
			// Reset game
			if (GetKey(olc::Key::SPACE).bPressed)
			{
				GameReset();
			}
		}
	}

	template<typename T>
	void CreateNewTerrainSegment(T scene, bool left, olc::vf2d distX, olc::vf2d distY)
	{
		// New rightNode is the previous back() segments leftNode
		// New leftNode is random with regard to previous back() segments leftNode
		if (left)
		{
			olc::vf2d newRightNode = scene->back().leftNode;
			olc::vf2d newLeftNode = { 
				scene->back().leftNode.x - RandFloat(distX.x, distX.y),
				scene->back().leftNode.y + RandFloat(distY.x, distY.y) 
			};

			scene->push_back({
				newLeftNode,
				newRightNode,
				GetGroundAngle(newLeftNode, newRightNode),
				false 
			});
		}
		else
		{
			olc::vf2d newLeftNode = scene->front().rightNode;
			olc::vf2d newRightNode = { 
				scene->front().rightNode.x + RandFloat(distX.x, distX.y),
				scene->front().rightNode.y + RandFloat(distY.x, distY.y) 
			};

			scene->push_front({
				newLeftNode,
				newRightNode,
				GetGroundAngle(newLeftNode, newRightNode),
				false 
			});
		}
	}

	void SpawnTerrain() 
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
			CreateNewTerrainSegment(&deqSegments, true, { 30.0f, 50.0f }, { -50.0f, 50.0f });

		if (deqSegments.front().leftNode.x < screenWidth + 250)
			CreateNewTerrainSegment(&deqSegments, false, { 30.0f, 50.0f }, { -50.0f, 50.0f });
		
		if (deqBgSegments.back().rightNode.x > -250)
			CreateNewTerrainSegment(&deqBgSegments, true, { 15.0f, 25.0f }, { -15.0f, 15.0f });
		
		if (deqBgSegments.front().leftNode.x < screenWidth + 250)
			CreateNewTerrainSegment(&deqBgSegments, false, { 15.0f, 25.0f }, { -15.0f, 15.0f });
	}

	void Draw(float fElapsedTime)
	{
		static float burnerTime = 0.0f;
		static float textTime = 0.0f;
		static float time = 0.0f;
		time += fElapsedTime;

		Clear(olc::BLACK);

		DrawSprite(earthPos * scale + adjustedPos, sprEarth.get(), int(scale + 1.0f));

		// Distinguish between ESC-pause and landed-pause
		if (paused && (int)playerAltitude) 
		{
			DrawString({ int(screenWidth * 0.33f), int(screenHeight * 0.7f) }, "         PAUSED\n\nPress SPACE to continue!");
			DrawString({ int(screenWidth * 0.05f), int(screenHeight * 0.1f) }, "Velocity", olc::DARK_GREY);
			DrawString({ int(screenWidth * 0.85f), int(screenHeight * 0.1f) }, "Fuel\nLandings", olc::DARK_GREY);
			DrawString({ int(screenWidth * 0.45f), int(screenHeight * 0.1f) }, "Score", olc::DARK_GREY);
			DrawString({ int(screenWidth * 0.55f), int(screenHeight * 0.4f), }, "Altitude", olc::DARK_GREY);
		}

		// Stars
		for (auto& star : vecStars)
		{
			if (star.lightIntensity < 0.25)
			{
				DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::VERY_DARK_GREY);
			}
			else if (star.lightIntensity < 0.5)
			{
				DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::DARK_GREY);
			}
			if (star.lightIntensity < 0.75)
			{
				DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::GREY);
			}
			else
			{
				DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::WHITE);
			}
		}

		// Background
		for (auto& segment : deqBgSegments)
		{
			// Find segments on screen
			if (segment.rightNode.x < screenWidth / scale - adjustedPos.x &&
				segment.leftNode.x > -screenWidth / scale + adjustedPos.x)
			{
				// Fill black
				DrawWarpedDecal(
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

		if (!playerDead)
		{
			DrawRotatedDecal(
				playerPos * scale + adjustedPos,
				decPlayer.get(),
				playerAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale
			);
		}
		else
		{
			// Different levels of damage decals
			if (normalizedHorizontalVelocity + normalizedVerticalVelocity < 7)
			{
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decPlayerLightDamage.get(),
					currentSegmentAngle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale
				);
			}
			else if (normalizedHorizontalVelocity + normalizedVerticalVelocity < 10)
			{
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decPlayerMediumDamage.get(),
					currentSegmentAngle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale
				);
			}
			else if (normalizedHorizontalVelocity + normalizedVerticalVelocity < 13)
			{
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decPlayerHeavyDamage.get(),
					currentSegmentAngle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale
				);
			}
			else if (normalizedHorizontalVelocity + normalizedVerticalVelocity > 12)
			{
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decPlayerDestroyed.get(),
					currentSegmentAngle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale
				);
			}
		}
		
		if ((int)playerFuel && !paused)
		{
			if (GetKey(olc::Key::W).bHeld)
			{
				burnerTime += fElapsedTime;
				if (burnerTime > 1.0f) 
					burnerTime = 1.0f;
				// Using sin() and time here for burner growth effect
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decBurnerEnd.get(),
					playerAngle,
					{ 8.0f, -8.0f },
					olc::vf2d( 0.8f + abs(sin(burnerTime)) * 0.5f, 0.8f + abs(sin(burnerTime)) * 0.5f ) * scale
				);
			}
			else
			{
				// Short burnoff decal, can't use bReleased because it only draws for one frame
				if (burnerTime > 0.0f)
					DrawRotatedDecal(
						playerPos * scale + adjustedPos,
						decEnd.get(),
						playerAngle,
						olc::vf2d( 8.0f, -5.0f ),
						olc::vf2d( 2.0f + abs(sin(burnerTime) * 3.0f), 1.5f) * scale
					);

				burnerTime -= fElapsedTime * 10;

				if (burnerTime < 0.0f) 
					burnerTime = 0.0f;
			}
			// Smaller side thrusters
			if (GetKey(olc::Key::A).bHeld)
			{
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decBurnerEnd.get(),
					playerAngle + 0.5f * PI,
					olc::vf2d(-25.0f, -15.0f),
					olc::vf2d(0.2f, 0.5f) * scale
				);
			}
			if (GetKey(olc::Key::D).bHeld)
			{
				DrawRotatedDecal(
					playerPos * scale + adjustedPos,
					decBurnerEnd.get(),
					playerAngle - 0.5f * PI,
					olc::vf2d(41.0f, -15.0f),
					olc::vf2d(0.2f, 0.5f) * scale
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
					DrawWarpedDecal(
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
					DrawWarpedDecal(
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
		// UI
		if (textTime < 3.0f)
		{
			DrawString({ int(screenWidth * 0.27f), int(screenHeight * 0.25f)}, "Land on the highlighted segments!");
			textTime += fElapsedTime;
		}

		std::vector<std::pair<olc::vf2d, std::string>> ui = {
			{ {screenWidth * 0.05f, screenHeight * 0.05f}, "H.V. " + std::to_string(normalizedHorizontalVelocity) + "m/s" },
			{ {screenWidth * 0.05f, screenHeight * 0.07f}, "V.V. " + std::to_string(normalizedVerticalVelocity) + "m/s" },
			{ {screenWidth * 0.85f, screenHeight * 0.05f}, "F " + std::to_string((int)playerFuel) + "kg"},
			{ {screenWidth * 0.85f, screenHeight * 0.07f}, "L " + std::to_string(playerLandings) },
			{ {screenWidth * 0.47f, screenHeight * 0.05f}, std::to_string((int)playerScore) },
			{ playerPos * scale + adjustedPos, std::to_string((int)playerAltitude) + "m" }
		};

		for (auto& line : ui)
		{
			if (line != ui.back())
				DrawString(line.first, line.second, olc::GREY);
			else
				if(!playerDead)
					DrawRotatedStringDecal(
						line.first,
						line.second,
						playerAngle,
						{ -10.0f, 10.0f },
						olc::GREY,
						olc::vf2d( 1.0f, 1.0f ) * scale
					);
		}
	}

	void CrewComms(float fElapsedTime)
	{
		const std::string command[5] = {
			"Eagle, status?", // random questions 0 - 4
			"This is command, status?",
			"How's it going up there?",
			"Eagle, sitrep.",
			"Status report, Eagle.",
		};

		const std::string crew[14] = {
			"This is Eagle, all good.", // responses 0-3
			"We're doing good out here!",
			"We could use some beer,\notherwise good!",
			"Command, we're just fine.",
			"Damn, nice view out here!", // random 4-6
			"I think I can see my\nhouse from here...",
			"Earth is beautiful...",
			"Carefully...", // low alt 7-9
			"Slowly...",
			"Steady...",
			"Capt. James: ", // names 10-12
			"John: ",
			"Fred: ",
			"\nWe're low on fuel!" // fuel
		};

		static float time = 0.0f;
		static float randomTime = RandFloat(50.0f, 70.0f);
		static bool randomVariablesSet = false;
		static bool isMessageOnScreen = false;
		static int randomControlQuestion;
		static int randomCrewReponse;
		static int randomCrewChatter;
		static int randomLowAltWarning;
		static int randomName;
		
		if (!randomVariablesSet || playerDead)
		{
			randomControlQuestion = rand() % 5;
			randomCrewReponse = rand() % 4;
			randomCrewChatter = rand() % 3 + 4;
			randomLowAltWarning = rand() % 3 + 7;
			randomName = rand() % 3 + 10;
			randomVariablesSet = true;
		}

		if (!paused)
			time += fElapsedTime;
		
		// Allow messsages in ESC-pause, but not landed-pause
		if (playerAltitude > 0.7f)
		{
			if (time > randomTime)
			{
				// Command messages
				if (time < randomTime + 8.0f)
					DrawStringDecal(
						{ screenWidth * 0.05f, screenHeight * 0.25f }, 
						"Command:\n" + command[randomControlQuestion], 
						olc::GREY
					);

				// Crew response
				if (time > randomTime + 3.0f && time < randomTime + 8.0f)
					DrawStringDecal(
						{ screenWidth * 0.05f, screenHeight * 0.3f },
						"Eagle:\n" + crew[randomCrewReponse],
						olc::GREY
					);

				// Reset timers when done
				if (time > randomTime + 8.0f)
				{
					randomVariablesSet = false;
					randomTime = RandFloat(50.0f, 70.0f);
					time = 0.0f;
				}
			}

			// Fuel
			if (playerFuel < 500)
				DrawStringDecal(
					{ screenWidth * 0.05f, screenHeight * 0.45f },
					crew[10] + crew[13],
					olc::GREY
				);

			// Crew chatter
			if (time > randomTime / 2 && time < randomTime / 2 + 7.0f && playerAltitude > 4.0f)
				DrawStringDecal(
					{ screenWidth * 0.05f, screenHeight * 0.35f },
					crew[randomName] + crew[randomCrewChatter], 
					olc::GREY
				);

			// Low alt
			if (playerAltitude < 4.0f && abs(currentSegmentAngle) <= 0.349f)
				DrawStringDecal(
					{ screenWidth * 0.05f, screenHeight * 0.4f },
					crew[randomName] + crew[randomLowAltWarning],
					olc::GREY
				);
			else
				randomLowAltWarning = rand() % 3 + 7;
		}
	}

	void DrawTitleScreen(float fElapsedTime)
	{
		// TODO: CONVERT TO VECTOR OF PAIRS LIKE Draw()
		DrawRotatedStringDecal( { screenWidth * 0.05f, screenHeight * 0.05f }, "   CONTROLS", 0.0f, { 0.0f, 0.0f }, olc::GREY);

		DrawRotatedStringDecal(
			{ screenWidth * 0.05f, screenHeight * 0.07f },
			"WAD   - Thrusters", 
			0.0f, 
			{ 0.0f, 0.0f }, 
			olc::GREY
		);

		DrawRotatedStringDecal(
			{ screenWidth * 0.05f, screenHeight * 0.09f },
			"SHIFT - Zoom", 
			0.0f, 
			{ 0.0f, 0.0f }, 
			olc::GREY
		);

		DrawRotatedStringDecal(
			{ screenWidth * 0.05f, screenHeight * 0.11f },
			"ESC   - Pause",
			0.0f, 
			{ 0.0f, 0.0f },
			olc::GREY
		);

		DrawString({ int(screenWidth * 0.25f), int(screenHeight * 0.3f) },
			"LUNAR LANDER", 
			olc::WHITE,
			3
		);

		DrawString(
			{ int(screenWidth * 0.35f), int(screenHeight * 0.8f) },
			"Press SPACE to start!", 
			olc::GREY);

		DrawRotatedDecal(
			{ screenWidth * 0.52f, screenHeight * 0.55f },
			decPlayer.get(), 
			0.4f, 
			{ 8.0f, 8.0f }, 
			{ 7.0f, 7.0f });

		DrawSprite(
			{ int(screenWidth * 0.8f), int(screenHeight * 0.1f) }, 
			sprEarth.get());

		if (GetKey(olc::SPACE).bPressed) titleScreen = false;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::ESCAPE).bPressed)
			paused = true;

		if (GetKey(olc::SPACE).bPressed)
			paused = false;

		if (titleScreen)
			DrawTitleScreen(fElapsedTime);
		else
		{
			SpawnTerrain();
			Draw(fElapsedTime);
			CrewComms(fElapsedTime);
			Physics(fElapsedTime);

			#ifdef DEBUG
			Debug();
			#endif
		}
		return true;
	}
};

int main() {

	lunarLander game;
	
	if (game.Construct(550, 390, 2, 2/*300, 240, 3, 3*/))
		game.Start();

	return 0;
}
#include "olcPixelGameEngine.h"
#include "player.h"
#include "global.h"
#include "terrain.h"
#include "background.h"
#include "interface.h"

Player::Player()
{
	position = { playerX, playerY };
	adjustedPosition = position / 2;

	sprPlayerLightDamage	= std::make_unique<olc::Sprite>("../gfx/landerLightDamage.png");
	sprPlayerMediumDamage	= std::make_unique<olc::Sprite>("../gfx/landerMediumDamage.png");
	sprPlayerHeavyDamage	= std::make_unique<olc::Sprite>("../gfx/landerHeavyDamage.png");
	sprPlayerDestroyed	= std::make_unique<olc::Sprite>("../gfx/landerDestroyed.png");
	sprPlayer	= std::make_unique<olc::Sprite>("../gfx/lander.png");
	sprBurner	= std::make_unique<olc::Sprite>("../gfx/burner.png");
	sprEnd	= std::make_unique<olc::Sprite>("../gfx/end.png");

	decPlayerLightDamage	= std::make_unique<olc::Decal>(sprPlayerLightDamage.get());
	decPlayerMediumDamage	= std::make_unique<olc::Decal>(sprPlayerMediumDamage.get());
	decPlayerHeavyDamage	= std::make_unique<olc::Decal>(sprPlayerHeavyDamage.get());
	decPlayerDestroyed	= std::make_unique<olc::Decal>(sprPlayerDestroyed.get());
	decPlayer	= std::make_unique<olc::Decal>(sprPlayer.get());
	decBurner	= std::make_unique<olc::Decal>(sprBurner.get());
	decEnd	= std::make_unique<olc::Decal>(sprEnd.get());

	Reset();
}

void Player::Draw(olc::PixelGameEngine* pge, float fElapsedTime)
{
	static float burnerTime = 0.0f;

	if (!dead)
		pge->DrawRotatedDecal(
			position * scale + adjustedPosition,
			decPlayer.get(),
			angle,
			olc::vf2d(8.0f, 8.0f),
			olc::vf2d(1.0f, 1.0f) * scale);
	else
	{
		// Different levels of damage decals
		if (normalizedHorizontalVelocity + normalizedVerticalVelocity < 7)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerLightDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);

		else if (normalizedHorizontalVelocity + normalizedVerticalVelocity < 10)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerMediumDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);

		else if (normalizedHorizontalVelocity + normalizedVerticalVelocity < 13)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerHeavyDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);
		
		else if (normalizedHorizontalVelocity + normalizedVerticalVelocity > 12)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerDestroyed.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);
	}

	if ((int)fuel && !paused)
	{
		if (pge->GetKey(olc::Key::W).bHeld)
		{
			burnerTime += fElapsedTime;
			if (burnerTime > 1.0f)
				burnerTime = 1.0f;
			// Using sin() and time here for burner growth effect
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decBurner.get(),
				angle,
				{ 8.0f, -8.0f },
				olc::vf2d(0.8f + abs(sin(burnerTime)) * 0.5f, 0.8f + abs(sin(burnerTime)) * 0.5f) * scale);
		}
		else
		{
			// Short burnoff decal, can't use bReleased because it only draws for one frame
			if (burnerTime > 0.0f)
				pge->DrawRotatedDecal(
					position * scale + adjustedPosition,
					decEnd.get(),
					angle,
					olc::vf2d(8.0f, -5.0f),
					olc::vf2d(2.0f + abs(sin(burnerTime) * 3.0f), 1.5f) * scale);

			burnerTime -= fElapsedTime * 10;

			if (burnerTime < 0.0f)
				burnerTime = 0.0f;
		}
		// Smaller side thrusters
		if (pge->GetKey(olc::Key::A).bHeld)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decBurner.get(),
				angle + 0.5f * PI,
				olc::vf2d(-25.0f, -15.0f),
				olc::vf2d(0.2f, 0.5f) * scale);

		if (pge->GetKey(olc::Key::D).bHeld)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decBurner.get(),
				angle - 0.5f * PI,
				olc::vf2d(41.0f, -15.0f),
				olc::vf2d(0.2f, 0.5f) * scale);
	}
}

void Player::LandingHandler(
	olc::PixelGameEngine* pge, 
	sSegment& segment, 
	Background* background,
	Terrain* terrain,
	Interface* userInterface,
	FileHandler* fileHandler)
{
	paused = true; // Pause game while landed
	int velocity = normalizedHorizontalVelocity + normalizedVerticalVelocity;
	
	// Successful landing
	if (normalizedHorizontalVelocity <= 3	&&
		normalizedVerticalVelocity <= 2		&&
		abs(segment.angle) <= 0.349f		&&		// 20 degrees
		!segment.visited					&&
		abs(angle - segment.angle) <= 0.087f)	// 5 degrees
	{
		userInterface->LandingMessages(pge, segment, velocity);

		if (pge->GetKey(olc::Key::SPACE).bPressed)
		{
			// Launch player based on ground angle
			this->velocity = { -cos(angle + HALFPI) * 60.0f, -sin(angle + HALFPI) * 60.0f };
			score += int(50 + abs(segment.angle) * 544 * (5 - velocity));
			fuel += 500;
			landings += 1;
			segment.visited = true;
			paused = false;
		}
	}
	else
	{
		dead = true;

		userInterface->DeathMessages(pge, fileHandler, velocity, int(score));

		if (pge->GetKey(olc::Key::SPACE).bPressed)
		{
			this->Reset();
			background->Reset();
			terrain->Reset();

			paused = false;
		}
	}
}

void Player::Physics(olc::PixelGameEngine* pge, Terrain* terrain, float fElapsedTime)
{
	static float Time = 0.0f;

	// Divide by 3 for believable velocity, this is what's displayed
	normalizedHorizontalVelocity = abs((int)(velocity.x / 3));
	normalizedVerticalVelocity = abs((int)(velocity.y / 3));

	if (!paused) // Disable everything if paused
	{
		// Angle reset if full circle
		if (abs(angle) > 6.283f) angle = 0.0f;

		// Directional velocity
		velocity += {
			thrust* cos(angle + PI * 0.5f)* fElapsedTime,
			thrust* sin(angle + PI * 0.5f)* fElapsedTime };

		// Gravity
		velocity.y += 15.0f * fElapsedTime;

		// +1 score per second, this is why score is float
		score += fElapsedTime;

		// Player is stationary, terrain moves inversely
		for (auto& segment : terrain->deqSegments)
		{
			segment.leftNode += velocity * -1.0f * fElapsedTime;
			segment.rightNode += velocity * -1.0f * fElapsedTime;
		}

		// Background moves slower than foreground
		for (auto& segment : terrain->deqBgSegments)
		{
			segment.leftNode += velocity * -0.3f * fElapsedTime;
			segment.rightNode += velocity * -0.3f * fElapsedTime;
		}

		if (pge->GetKey(olc::Key::W).bHeld && (int)fuel)
		{
			thrust = -40.0f;
			fuel -= 100.0f * fElapsedTime;
		}
		else
		{
			thrust = 0.0f;
		}

		if (pge->GetKey(olc::Key::A).bHeld && (int)fuel)
		{
			angle -= 1.5f * fElapsedTime;
			fuel -= 10.0f * fElapsedTime;
		}

		if (pge->GetKey(olc::Key::D).bHeld && (int)fuel)
		{
			angle += 1.5f * fElapsedTime;
			fuel -= 10.0f * fElapsedTime;
		}
	}

	// Shift to change zoom
	if (pge->GetKey(olc::SHIFT).bPressed)
	{
		if (scale == 1.5f)
		{
			scale = 0.5f;
			// Adjusted position used in Draw() so everything stays centered
			adjustedPosition = position * 0.5f; 
		}
		else
		{
			scale = 1.5f;
			adjustedPosition = position * -0.5f;
		}
	}
}

void Player::Reset()
{
	angle	= 0.0f;
	position	= { (float)screenWidth / 2, (float)screenHeight / 2 };
	velocity	= { 0.0f, 0.7f };
	thrust	= 0.0f;
	score	= 0;
	fuel	= 2250;
	landings	= 0;
	dead	= false;
}
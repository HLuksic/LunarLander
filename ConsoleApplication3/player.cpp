#include "player.h"
#include "global.h"
#include "terrain.h"
#include "background.h"
#include "interface.h"
#include "audio.h"

Player::Player()
{
	position              = { PLAYER_X, PLAYER_Y };
	adjustedPosition      = position / 2;

	sprPlayerLightDamage  = std::make_unique<olc::Sprite>("../gfx/landerLightDamage.png");
	sprPlayerMediumDamage = std::make_unique<olc::Sprite>("../gfx/landerMediumDamage.png");
	sprPlayerHeavyDamage  = std::make_unique<olc::Sprite>("../gfx/landerHeavyDamage.png");
	sprPlayerDestroyed    = std::make_unique<olc::Sprite>("../gfx/landerDestroyed.png");
	sprPlayer             = std::make_unique<olc::Sprite>("../gfx/lander.png");
	sprBurner             = std::make_unique<olc::Sprite>("../gfx/burner.png");
	sprEnd                = std::make_unique<olc::Sprite>("../gfx/end.png");
	decPlayerLightDamage  = std::make_unique<olc::Decal>(sprPlayerLightDamage.get());
	decPlayerMediumDamage = std::make_unique<olc::Decal>(sprPlayerMediumDamage.get());
	decPlayerHeavyDamage  = std::make_unique<olc::Decal>(sprPlayerHeavyDamage.get());
	decPlayerDestroyed    = std::make_unique<olc::Decal>(sprPlayerDestroyed.get());
	decPlayer             = std::make_unique<olc::Decal>(sprPlayer.get());
	decBurner             = std::make_unique<olc::Decal>(sprBurner.get());
	decEnd                = std::make_unique<olc::Decal>(sprEnd.get());

	Reset();
}

void Player::Draw(olc::PixelGameEngine* pge, float fElapsedTime)
{
	static float mainBurnerTime = 0.0f;
	static float sideBurnerTime = 0.0f;

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
		if (normHorVel + normVerVel < 7)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerLightDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);

		else if (normHorVel + normVerVel < 10)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerMediumDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);

		else if (normHorVel + normVerVel < 13)
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decPlayerHeavyDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);
		
		else if (normHorVel + normVerVel > 12)
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
			mainBurnerTime += fElapsedTime;

			if (mainBurnerTime > 1.0f)
				mainBurnerTime = 1.0f;

			// Using sin() and time here for burner growth effect
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decBurner.get(),
				angle,
				{ 8.0f, -8.0f },
				olc::vf2d(0.8f + abs(sin(mainBurnerTime)) * 0.5f, 0.8f + abs(sin(mainBurnerTime)) * 0.5f) * scale);
		}
		else
		{
			// Short burnoff effect
			if (mainBurnerTime > 0.0f)
				pge->DrawRotatedDecal(
					position * scale + adjustedPosition,
					decEnd.get(),
					angle,
					olc::vf2d(8.0f, -5.0f),
					olc::vf2d(2.0f + abs(sin(mainBurnerTime) * 3.0f), 1.5f) * scale);

			mainBurnerTime -= fElapsedTime * 10;

			if (mainBurnerTime < 0.0f)
				mainBurnerTime = 0.0f;
		}

		// Side thrusters
		if (pge->GetKey(olc::Key::A).bHeld)
		{
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decBurner.get(),
				angle + 0.5f * PI,
				olc::vf2d(-25.0f, -15.0f),
				olc::vf2d(0.2f, 0.5f) * scale);

			sideBurnerTime = 0.3f;
		}
		else // Counter-thrusters
			if (sideBurnerTime > 0.0f)
			{
				pge->DrawRotatedDecal(
					position * scale + adjustedPosition,
					decBurner.get(),
					angle - 0.5f * PI,
					olc::vf2d(41.0f, -15.0f),
					olc::vf2d(0.2f, 0.5f) * scale);

				sideBurnerTime -= fElapsedTime;
			}

		if (pge->GetKey(olc::Key::D).bHeld)
		{
			pge->DrawRotatedDecal(
				position * scale + adjustedPosition,
				decBurner.get(),
				angle - 0.5f * PI,
				olc::vf2d(41.0f, -15.0f),
				olc::vf2d(0.2f, 0.5f) * scale);

			sideBurnerTime = 0.3f;
		}
		else
			if (sideBurnerTime > 0.0f)
			{
				pge->DrawRotatedDecal(
					position * scale + adjustedPosition,
					decBurner.get(),
					angle + 0.5f * PI,
					olc::vf2d(-25.0f, -15.0f),
					olc::vf2d(0.2f, 0.5f)* scale);

				sideBurnerTime -= fElapsedTime;
			}
	}
}

void Player::HandleLanding(
	olc::PixelGameEngine* pge, 
	sSegment& segment, 
	Background* _Background,
	Terrain* _Terrain,
	Audio* _Audio,
	float fElapsedTime)
{
	static bool statsUpdated = false;

	paused = true;
	
	// Successful landing
	if (LandingSuccessful(segment))
	{
		gainedScore = int(50 + abs(segment.angle) * 544 * (5 - (normHorVel + normVerVel)));
		
		_Audio->PlaySoundSample(pge, 3, 3);
		
		if (!statsUpdated)
		{
			score       += gainedScore;
			fuel        += 500;
			landings    += 1;
			statsUpdated = true;
		}

		if (pge->GetKey(olc::Key::SPACE).bPressed)
		{
			_Audio->soundPlayed = false;
			_Audio->PlaySoundSample(pge, 1, 2);
			_Audio->soundPlayed = false;

			// Launch player based on ground angle
			this->velocity  = { -cos(angle + HALF_PI) * 90.0f, -sin(angle + HALF_PI) * 90.0f };
			paused          = false;
			statsUpdated    = false;
			segment.visited = true;
		}
	}
	else
	{
		dead = true;

		_Audio->PlaySoundSample(pge, 2, 9);

		if (pge->GetKey(olc::Key::SPACE).bPressed)
		{
			_Audio->soundPlayed = false;
			_Audio->PlaySoundSample(pge, 1, 11);
			_Audio->soundPlayed = false;

			this->Reset();
			_Background->Reset();
			_Terrain->Reset();
			adjustedPosition = position * 0.5f;
			scale            = 0.5f;
			paused           = false;
		}
	}
}

void Player::Physics(olc::PixelGameEngine* pge, Terrain* _Terrain, Audio* _Audio, float fElapsedTime)
{
	static float Time = 0.0f;

	if (!paused)
	{
		if (fuel > 2250.0f)
			fuel = 2250.0f;

		// Divide by 3 for believable velocity, this is what's displayed
		normHorVel = abs((int)(velocity.x / 3));
		normVerVel = abs((int)(velocity.y / 3));

		// Angle reset if full circle
		if (abs(angle) > 6.283f) angle = 0.0f;

		// Directional velocity
		velocity += { thrust * cos(angle + PI * 0.5f) * fElapsedTime, thrust * sin(angle + PI * 0.5f) * fElapsedTime };

		// Gravity
		velocity.y += 15.0f * fElapsedTime;

		// +1 score per second, score has to be float for this to work
		score += fElapsedTime;

		if (pge->GetKey(olc::Key::W).bHeld && (int)fuel)
		{
			thrust = -40.0f;
			fuel  -= 100.0f * fElapsedTime; // -100 fuel/second
		}
		else
			thrust = 0.0f;

		if (pge->GetKey(olc::Key::A).bHeld && (int)fuel)
		{
			angle -= 1.5f * fElapsedTime;
			fuel  -= 10.0f * fElapsedTime;
		}

		if (pge->GetKey(olc::Key::D).bHeld && (int)fuel)
		{
			angle += 1.5f * fElapsedTime;
			fuel  -= 10.0f * fElapsedTime;
		}
	}

	// Shift to change zoom
	if (pge->GetKey(olc::SHIFT).bPressed)
	{
		if (scale == 1.5f)
		{
			scale = 0.5f;
			// Adjusted position so everything keeps relative position
			adjustedPosition = position * 0.5f; 
		}
		else
		{
			scale = 1.5f;
			adjustedPosition = position * -0.5f;
		}
	}

	static bool lowFuelSoundPlayed;

	if ((int)fuel > 500.0f)
		lowFuelSoundPlayed = false;

	if ((int)fuel < 500.0f && !lowFuelSoundPlayed)
	{
		_Audio->PlaySoundSample(pge, 1, 12);
		_Audio->soundPlayed = false;
		lowFuelSoundPlayed  = true;
	}
}

void Player::Reset()
{
	angle    = 0.0f;
	position = { (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 };
	velocity = { 0.0f, 0.7f };
	thrust   = 0.0f;
	score    = 0;
	fuel     = 2250;
	landings = 0;
	dead     = false;
}

bool Player::LandingSuccessful(sSegment& segment)
{
	return (normHorVel                   <= 3      &&
            normVerVel                   <= 2      &&
            abs(segment.angle)           <= 0.349f && // 20 degrees
            abs(angle - segment.angle)   <= 0.087f && // 5 degrees
	        !segment.visited);
}
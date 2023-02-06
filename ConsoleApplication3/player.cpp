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
	{
		pge->DrawRotatedDecal(
			position * Scale + adjustedPosition,
			decPlayer.get(),
			angle,
			olc::vf2d(8.0f, 8.0f),
			olc::vf2d(1.0f, 1.0f) * Scale);
	}
	else
	{
		// Different levels of damage decals
		if (normHorVel + normVerVel < 7)
		{
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decPlayerLightDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * Scale);
		}
		else if (normHorVel + normVerVel < 10)
		{
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decPlayerMediumDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * Scale);
		}
		else if (normHorVel + normVerVel < 13)
		{
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decPlayerHeavyDamage.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * Scale);
		}
		else
		{
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decPlayerDestroyed.get(),
				currentSegmentAngle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * Scale);
		}
	}

	if ((int)fuel && !Paused)
	{
		if (pge->GetKey(olc::Key::W).bHeld)
		{
			mainBurnerTime += fElapsedTime;

			if (mainBurnerTime > 1.0f)
			{
				mainBurnerTime = 1.0f;
			}

			// Using sine and time here for exhaust growth effect
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decBurner.get(),
				angle,
				{ 8.0f, -8.0f },
				olc::vf2d(0.8f + abs(sin(mainBurnerTime)) * 0.5f, 0.8f + abs(sin(mainBurnerTime)) * 0.5f) * Scale);
		}
		else
		{
			// Short burnoff effect
			if (mainBurnerTime > 0.0f)
			{
				pge->DrawRotatedDecal(
					position * Scale + adjustedPosition,
					decEnd.get(),
					angle,
					olc::vf2d(8.0f, -5.0f),
					olc::vf2d(2.0f + abs(sin(mainBurnerTime) * 3.0f), 1.5f) * Scale);
			}

			mainBurnerTime -= fElapsedTime * 10;

			if (mainBurnerTime < 0.0f)
			{
				mainBurnerTime = 0.0f;
			}
		}

		// Side thrusters
		if (pge->GetKey(olc::Key::A).bHeld)
		{
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decBurner.get(),
				angle + 0.5f * PI,
				olc::vf2d(-25.0f, -15.0f),
				olc::vf2d(0.2f, 0.5f) * Scale);

			sideBurnerTime = 0.3f;
		}
		else // Counter-thrusters
			if (sideBurnerTime > 0.0f)
			{
				pge->DrawRotatedDecal(
					position * Scale + adjustedPosition,
					decBurner.get(),
					angle - 0.5f * PI,
					olc::vf2d(41.0f, -15.0f),
					olc::vf2d(0.2f, 0.5f) * Scale);

				sideBurnerTime -= fElapsedTime;
			}

		if (pge->GetKey(olc::Key::D).bHeld)
		{
			pge->DrawRotatedDecal(
				position * Scale + adjustedPosition,
				decBurner.get(),
				angle - 0.5f * PI,
				olc::vf2d(41.0f, -15.0f),
				olc::vf2d(0.2f, 0.5f) * Scale);

			sideBurnerTime = 0.3f;
		}
		else
			if (sideBurnerTime > 0.0f)
			{
				pge->DrawRotatedDecal(
					position * Scale + adjustedPosition,
					decBurner.get(),
					angle + 0.5f * PI,
					olc::vf2d(-25.0f, -15.0f),
					olc::vf2d(0.2f, 0.5f)* Scale);

				sideBurnerTime -= fElapsedTime;
			}
	}
}

void Player::HandleLanding(olc::PixelGameEngine* pge, Segment& segment, 
	Background* _Background, Terrain* _Terrain, Audio* _Audio, float fElapsedTime)
{
	static bool statsUpdated = false;
	float segmentAngle = _Terrain->GetGroundAngle(segment.leftNode, segment.rightNode);
    Paused             = true;
	
	if (LandingSuccessful(segment, segmentAngle))
	{
		_Audio->PlaySampleOnce(pge, 3, 3);
		UpdateScore(segmentAngle, statsUpdated, fElapsedTime);

		if (pge->GetKey(olc::Key::SPACE).bPressed)
		{
			_Audio->soundPlayed = false;
			_Audio->PlaySampleOnce(pge, 1, 2);
			_Audio->soundPlayed = false;

			// Launch player based on ground angle
			velocity        = { -cos(angle + HALF_PI) * 90.0f, -sin(angle + HALF_PI) * 90.0f };
			Paused          = false;
			statsUpdated    = false;
			segment.visited = true;
			landings++;

			// Reset terrain at high score to prevent huge deque
			if (score > 10000)
			{
				velocity = { 0.0f, 0.0f };
				_Terrain->deqBgSegments.clear();
				_Terrain->deqFgSegments.clear();
			}
		}
	}
	else
	{
		dead = true;

		_Audio->PlaySampleOnce(pge, 2, 9);

		if (pge->GetKey(olc::Key::SPACE).bPressed)
		{
			_Audio->soundPlayed = false;
			_Audio->PlaySampleOnce(pge, 1, 11);
			_Audio->soundPlayed = false;

			this->Reset();
			_Background->Reset();
			_Terrain->Reset();
			Paused = false;
		}
	}
}

void Player::UpdateScore(float segmentAngle, bool& statsUpdated, float fElapsedTime)
{
	
	// Calculate score based on angle of terrain and angle of landing, max. 1000
	gainedScore = int(50 + abs(segmentAngle) * 544 * (5 - (normHorVel + normVerVel)));

	// This is slightly inaccurate sometimes, but I don't care
	if (!statsUpdated)
	{
		static double t = 1.0;
		t -= fElapsedTime;
		score += gainedScore * fElapsedTime;
		fuel += 500.0f * fElapsedTime;

		if (t <= 0.0)
		{
			t = 1.0;
			statsUpdated = true;
		}
	}
}

void Player::Physics(olc::PixelGameEngine* pge, Terrain* _Terrain, Audio* _Audio, float fElapsedTime)
{
	if (fuel > 2250.0f)
		fuel = 2250.0f;

	if (!Paused)
	{
		// Divide by 3 for believable velocity, this is what's displayed
		normHorVel = abs((int)(velocity.x / 3));
		normVerVel = abs((int)(velocity.y / 3));

		// Angle reset if full circle
		if (abs(angle) > DegToRad(360.0f)) angle = 0.0f;

		// Directional velocity
		velocity += { thrust * cos(angle + PI * 0.5f) * fElapsedTime, thrust * sin(angle + PI * 0.5f) * fElapsedTime };

		// Gravity
		velocity.y += 15.0f * fElapsedTime;

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
		if (Scale == 1.5f)
		{
			Scale = 0.5f;
			// Adjusted position so everything keeps relative position
			adjustedPosition = position * 0.5f; 
		}
		else
		{
			Scale = 1.5f;
			adjustedPosition = position * -0.5f;
		}
	}

	static bool lowFuelSoundPlayed;

	if ((int)fuel > 500.0f)
		lowFuelSoundPlayed = false;

	if ((int)fuel < 500.0f && !lowFuelSoundPlayed)
	{
		_Audio->PlaySampleOnce(pge, 1, 12);
		_Audio->soundPlayed = false;
		lowFuelSoundPlayed  = true;
	}
}

void Player::Reset()
{
	adjustedPosition = position * 0.5f;
	Scale            = 0.5f;
	angle            = 0.0f;
	velocity         = { 0.0f, 0.7f };
	thrust           = 0.0f;
	score            = 0;
	fuel             = 2250;
	landings         = 0;
	dead             = false;
}

bool Player::LandingSuccessful(Segment& segment, float segmentAngle)
{
	return (normHorVel                <= 3               &&
	        normVerVel                <= 2               &&
	        abs(angle)                <= DegToRad(20.0f) &&
	        abs(angle - segmentAngle) <= DegToRad(5.0f)  &&
	        !segment.visited);
}
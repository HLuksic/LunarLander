#include "olcPixelGameEngine.h"

#include "background.h"
#include "global.h"
#include "player.h"

Background::Background()
{
	sprEarth = std::make_unique<olc::Sprite>("../gfx/earth.png");
	Reset();
}

void Background::Draw(olc::PixelGameEngine* pge, Player* player)
{
	pge->DrawSprite(earthPos * scale + player->adjustedPos, sprEarth.get(), int(scale + 1.0f));

	for (auto& star : vecStars)
	{
		if (star.lightIntensity < 0.25)
		{
			pge->DrawCircle(star.position * scale + player->adjustedPos, 1 * scale, olc::VERY_DARK_GREY);
		}
		else if (star.lightIntensity < 0.5)
		{
			pge->DrawCircle(star.position * scale + player->adjustedPos, 1 * scale, olc::DARK_GREY);
		}
		if (star.lightIntensity < 0.75)
		{
			pge->DrawCircle(star.position * scale + player->adjustedPos, 1 * scale, olc::GREY);
		}
		else
		{
			pge->DrawCircle(star.position * scale + player->adjustedPos, 1 * scale, olc::WHITE);
		}
	}
}

void Background::Reset()
{
	vecStars.clear();

	for (int i = 0; i < 50; i++)
	{
		sStar star;
		star.position = olc::vf2d(RandFloat(-250.0f, 800.0f), RandFloat(-110.0f, 305.0f));
		star.lightIntensity = RandFloat(0.0f, 1.0f);

		vecStars.push_back(star);
	}

	earthPos = { RandFloat(100.0f, 400.0f), RandFloat(100.0f, 150.0f) };
}
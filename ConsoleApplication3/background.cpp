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
	for (auto& star : vecStars)
	{
		if (star.lightIntensity < 0.25)
			pge->DrawCircle(star.position * scale + player->adjustedPosition, 1 * int(scale), olc::VERY_DARK_GREY);
		else if (star.lightIntensity < 0.5)
			pge->DrawCircle(star.position * scale + player->adjustedPosition, 1 * int(scale), olc::DARK_GREY);
		else if (star.lightIntensity < 0.75)
			pge->DrawCircle(star.position * scale + player->adjustedPosition, 1 * int(scale), olc::GREY);
		else
			pge->DrawCircle(star.position * scale + player->adjustedPosition, 1 * int(scale), olc::WHITE);
	}

	pge->DrawSprite(earthPos * scale + player->adjustedPosition, sprEarth.get(), int(scale * 2));
}

void Background::Reset()
{
	vecStars.clear();

	for (int i = 0; i < 100; i++)
	{
		sStar star;
		star.position = olc::vf2d(RandFloat(-250.0f, 800.0f), RandFloat(-140.0f, 295.0f));
		star.lightIntensity = RandFloat(0.0f, 1.0f);

		vecStars.push_back(star);
	}

	earthPos = { RandFloat(350.0f, 420.0f), RandFloat(100.0f, 200.0f) };
}
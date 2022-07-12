#include "background.h"
#include "global.h"
#include "player.h"

Background::Background()
{
	sprEarth = std::make_unique<olc::Sprite>("../gfx/earth.png");
	decEarth = std::make_unique<olc::Decal>(sprEarth.get());
	Reset();
}

void Background::Draw(olc::PixelGameEngine* pge, Player* _Player)
{
	for (auto& star : vecStars)
	{
		if (star.lightIntensity < 0.25)
			pge->DrawCircle(star.position * Scale + _Player->adjustedPosition, 1 * int(Scale), olc::VERY_DARK_GREY);
		else if (star.lightIntensity < 0.5)
			pge->DrawCircle(star.position * Scale + _Player->adjustedPosition, 1 * int(Scale), olc::DARK_GREY);
		else if (star.lightIntensity < 0.75)
			pge->DrawCircle(star.position * Scale + _Player->adjustedPosition, 1 * int(Scale), olc::GREY);
		else
			pge->DrawCircle(star.position * Scale + _Player->adjustedPosition, 1 * int(Scale), olc::WHITE);
	}

	pge->DrawDecal(earthPos * Scale + _Player->adjustedPosition, decEarth.get(), { Scale * 2, Scale * 2 });
}

void Background::Reset()
{
	vecStars.clear();

	for (int i = 0; i < 100; i++)
	{
		sStar star;
		star.position = olc::vf2d(RandFloat(-250.0f, 800.0f), RandFloat(-140.0f, 310.0f));
		star.lightIntensity = RandFloat(0.0f, 1.0f);

		vecStars.push_back(star);
	}

	earthPos = { RandFloat(350.0f, 420.0f), RandFloat(100.0f, 200.0f) };
}
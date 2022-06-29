
#include "background.h"
#include "global.h"

Background::Background()
{
	sprEarth = std::make_unique<olc::Sprite>("../gfx/earth.png");
	Reset();
}

void Background::Draw(olc::PixelGameEngine* pge)
{
	pge->DrawSprite(earthPos * scale + adjustedPos, sprEarth.get(), int(scale + 1.0f));

	for (auto& star : vecStars)
	{
		if (star.lightIntensity < 0.25)
		{
			pge->DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::VERY_DARK_GREY);
		}
		else if (star.lightIntensity < 0.5)
		{
			pge->DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::DARK_GREY);
		}
		if (star.lightIntensity < 0.75)
		{
			pge->DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::GREY);
		}
		else
		{
			pge->DrawCircle(star.position * scale + adjustedPos, 1 * scale, olc::WHITE);
		}
	}
}

void Background::Reset()
{
	vecStars.clear();

	for (int i = 0; i < 50; i++)
	{
		sStar star;
		star.position = olc::vf2d(RandFloat(-225.0f, 775.0f), RandFloat(-98.0f, 293.0f));
		star.lightIntensity = RandFloat(0.0f, 1.0f);

		vecStars.push_back(star);
	}

	earthPos = { RandFloat(100.0f, 400.0f), RandFloat(100.0f, 150.0f) };
}
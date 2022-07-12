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
		pge->DrawCircle(star.position * Scale + _Player->adjustedPosition, 1 * int(Scale), olc::Pixel(255, 255, 255, star.lightIntensity));

	pge->DrawDecal(earthPos * Scale + _Player->adjustedPosition, decEarth.get(), { Scale * 2, Scale * 2 });
}

void Background::Reset()
{
	vecStars.clear();

	for (int i = 0; i < 100; i++)
	{
		sStar star;
		star.position = olc::vf2d(RandInRange(-250.0f, 800.0f), RandInRange(-140.0f, 310.0f));
		star.lightIntensity = uint8_t(RandInRange(0.0f, 255.0f));

		vecStars.push_back(star);
	}

	earthPos = { RandInRange(350.0f, 420.0f), RandInRange(100.0f, 200.0f) };
}
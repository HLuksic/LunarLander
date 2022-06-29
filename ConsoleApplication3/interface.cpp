#include "interface.h"
#include "global.h"
#include "player.h"

void Interface::Draw(olc::PixelGameEngine* pge, float fElapsedTime)
{
	static float textTime = 0.0f;

	/**************
	* Main UI
	**************/
	// Distinguish between ESC-pause and landed-pause
	if (paused && (int)Player::playerAltitude)
	{
		pge->DrawString({ int(screenWidth * 0.33f), int(screenHeight * 0.7f) }, "         PAUSED\n\nPress SPACE to continue!");
		pge->DrawString({ int(screenWidth * 0.05f), int(screenHeight * 0.1f) }, "Velocity", olc::DARK_GREY);
		pge->DrawString({ int(screenWidth * 0.85f), int(screenHeight * 0.1f) }, "Fuel\nLandings", olc::DARK_GREY);
		pge->DrawString({ int(screenWidth * 0.45f), int(screenHeight * 0.1f) }, "Score", olc::DARK_GREY);
		pge->DrawString({ int(screenWidth * 0.55f), int(screenHeight * 0.4f), }, "Altitude", olc::DARK_GREY);
	}

	if (textTime < 3.0f)
	{
		pge->DrawString({ int(screenWidth * 0.27f), int(screenHeight * 0.25f) }, "Land on the highlighted segments!");
		textTime += fElapsedTime;
	}

	std::vector<std::pair<olc::vf2d, std::string>> ui = {
		{ {screenWidth * 0.05f, screenHeight * 0.05f}, "H.V. " + std::to_string(Player::normalizedHorizontalVelocity) + "m/s" },
		{ {screenWidth * 0.05f, screenHeight * 0.07f}, "V.V. " + std::to_string(Player::normalizedVerticalVelocity) + "m/s" },
		{ {screenWidth * 0.85f, screenHeight * 0.05f}, "F " + std::to_string((int)Player::playerFuel) + "kg"},
		{ {screenWidth * 0.85f, screenHeight * 0.07f}, "L " + std::to_string(Player::playerLandings) },
		{ {screenWidth * 0.47f, screenHeight * 0.05f}, std::to_string((int)Player::playerScore) },
		{ playerPos * scale + adjustedPos, std::to_string((int)Player::playerAltitude) + "m" }
	};

	for (auto& line : ui)
	{
		if (line != ui.back())
			pge->DrawString(line.first, line.second, olc::GREY);
		else
			if (!Player::playerDead)
				pge->DrawRotatedStringDecal(
					line.first,
					line.second,
					Player::playerAngle,
					{ -10.0f, 10.0f },
					olc::GREY,
					olc::vf2d(1.0f, 1.0f) * scale
				);
	}

	/**************
	* Comms
	**************/
	static float time = 0.0f;
	static float randomTime = RandFloat(50.0f, 70.0f);
	static bool randomVariablesSet = false;
	static bool isMessageOnScreen = false;
	static int randomControlQuestion;
	static int randomCrewReponse;
	static int randomCrewChatter;
	static int randomLowAltWarning;
	static int randomName;

	if (!randomVariablesSet || Player::playerDead)
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
	if (Player::playerAltitude > 0.7f)
	{
		if (time > randomTime)
		{
			// Command messages
			if (time < randomTime + 8.0f)
				pge->DrawStringDecal(
					{ screenWidth * 0.05f, screenHeight * 0.25f },
					"Command:\n" + command[randomControlQuestion],
					olc::GREY
				);

			// Crew response
			if (time > randomTime + 3.0f && time < randomTime + 8.0f)
				pge->DrawStringDecal(
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
		if (Player::playerFuel < 500)
			pge->DrawStringDecal(
				{ screenWidth * 0.05f, screenHeight * 0.45f },
				crew[10] + crew[13],
				olc::GREY
			);

		// Crew chatter
		if (time > randomTime / 2 && time < randomTime / 2 + 7.0f && Player::playerAltitude > 4.0f)
			pge->DrawStringDecal(
				{ screenWidth * 0.05f, screenHeight * 0.35f },
				crew[randomName] + crew[randomCrewChatter],
				olc::GREY
			);

		// Low alt
		if (Player::playerAltitude < 4.0f && abs(Player::currentSegmentAngle) <= 0.349f)
			pge->DrawStringDecal(
				{ screenWidth * 0.05f, screenHeight * 0.4f },
				crew[randomName] + crew[randomLowAltWarning],
				olc::GREY
			);
		else
			randomLowAltWarning = rand() % 3 + 7;
	}
}

void Interface::TitleScreen(olc::PixelGameEngine* pge)
{
	//TODO: covnert to vector of pairs like draw

	pge->DrawRotatedStringDecal({ screenWidth * 0.05f, screenHeight * 0.05f }, "   CONTROLS", 0.0f, { 0.0f, 0.0f }, olc::GREY);

	pge->DrawRotatedStringDecal(
		{ screenWidth * 0.05f, screenHeight * 0.07f },
		"WAD   - Thrusters",
		0.0f,
		{ 0.0f, 0.0f },
		olc::GREY
	);

	pge->DrawRotatedStringDecal(
		{ screenWidth * 0.05f, screenHeight * 0.09f },
		"SHIFT - Zoom",
		0.0f,
		{ 0.0f, 0.0f },
		olc::GREY
	);

	pge->DrawRotatedStringDecal(
		{ screenWidth * 0.05f, screenHeight * 0.11f },
		"ESC   - Pause",
		0.0f,
		{ 0.0f, 0.0f },
		olc::GREY
	);

	pge->DrawString({ int(screenWidth * 0.25f), int(screenHeight * 0.3f) },
		"LUNAR LANDER",
		olc::WHITE,
		3
	);

	pge->DrawString(
		{ int(screenWidth * 0.35f), int(screenHeight * 0.8f) },
		"Press SPACE to start!",
		olc::GREY);

	pge->DrawRotatedDecal(
		{ screenWidth * 0.52f, screenHeight * 0.55f },
		Player::decPlayer.get(),
		0.4f,
		{ 8.0f, 8.0f },
		{ 7.0f, 7.0f });

	if (pge->GetKey(olc::SPACE).bPressed) titleScreen = false;
}

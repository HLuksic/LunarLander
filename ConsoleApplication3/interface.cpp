#include "interface.h"
#include "global.h"
#include "terrain.h"
#include "background.h"
#include "filehandler.h"


Interface::Interface() :
	command
	{
		"Eagle, status?",                               // random questions 0 - 4
		"This is command, status?",
		"How's it going up there?",
		"Eagle, sitrep.",
		"Status report, Eagle.",
	},
	crew
	{
		"This is Eagle, all good.",                     // responses 0-3
		"We're doing good\nout here!",
		"We could use some beer,\notherwise good!",
		"Command, we're just fine.",
		"Damn, nice view\nout here!",                   // random 4-6
		"I think I can\nsee my house from here...",
		"Earth is beautiful...",
		"Carefully...",                                 // low alt 7-9
		"Slowly...",
		"Steady...",
		"Capt. James: ",                                // names 10-12
		"John: ",
		"Fred: ",
		"\nWe're low on fuel!"                          // fuel
	}
{
	sprBar      = std::make_unique<olc::Sprite>("../gfx/bar.png");
	decBar      = std::make_unique<olc::Decal>(sprBar.get());
	titleScreen = true;
}

void Interface::Draw(olc::PixelGameEngine* pge, Player* _Player, FileHandler* _FileHandler, float fElapsedTime)
{
	FuelGauge(pge, _Player);
	Comms(pge, _Player, fElapsedTime);

	static float textTime = 0.0f;

	if (textTime < 3.0f)
	{
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.27f), int(SCREEN_HEIGHT * 0.25f) }, "Land on the highlighted segments!");
		textTime += fElapsedTime;
	}

	// Distinguish between ESC-pause and landed-pause
	if (paused && (int)_Player->altitude)
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.33f), int(SCREEN_HEIGHT * 0.7f) }, "         PAUSED\n\nPress SPACE to continue!");

	const std::vector<std::pair<olc::vf2d, std::string>> ui = 
	{
		{ _Player->position * scale + _Player->adjustedPosition,   std::to_string((int)_Player->altitude) + "m" }, 
		{ {SCREEN_WIDTH * 0.03f, SCREEN_HEIGHT * 0.03f}, "H.V. " + std::to_string(_Player->normHorVel) + "m/s" },
		{ {SCREEN_WIDTH * 0.03f, SCREEN_HEIGHT * 0.05f}, "V.V. " + std::to_string(_Player->normVerVel) + "m/s" },
		{ {SCREEN_WIDTH * 0.88f, SCREEN_HEIGHT * 0.03f}, "S "    + std::to_string((int)_Player->score)},
		{ {SCREEN_WIDTH * 0.88f, SCREEN_HEIGHT * 0.05f}, "L "    + std::to_string(_Player->landings) }
	};

	for (auto& line : ui)
	{
		if (line != ui.front())
			pge->DrawStringDecal(line.first, line.second, olc::GREY);
		else
			if (!_Player->dead)
				pge->DrawRotatedStringDecal(line.first, line.second, _Player->angle, { -10.0f, 10.0f }, olc::GREY, olc::vf2d(1.0f, 1.0f) * scale);
	}

	if (paused && !_Player->dead && _Player->altitude < 0.7f)
		LandingMessages(pge, _Player->normHorVel + _Player->normVerVel, _Player->gainedScore);

	if (paused && _Player->dead)
		DeathMessages(pge, _FileHandler, _Player->normHorVel + _Player->normVerVel, (int)_Player->score);
}

void Interface::TitleScreen(olc::PixelGameEngine* pge, Background* _Background, Player* _Player, FileHandler* _FileHandler, Audio* _Audio)
{
	const std::vector<std::pair<olc::vf2d, std::string>> ui = 
	{
		{ { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.3f  },  "LUNAR LANDER" },
		{ { SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.05f }, "   CONTROLS" },
		{ { SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.07f }, "W,A,D - Thrusters" },
		{ { SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.09f }, "SHIFT - Zoom" },
		{ { SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.11f }, "ESC   - Pause" },
		{ { SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.8f  }, "Press SPACE to start!" }
	};

	for (auto& line : ui)
	{
		if (line != ui.front())
			pge->DrawStringDecal(line.first, line.second, olc::GREY);
		else
			pge->DrawStringDecal(line.first, line.second, olc::GREY, { 3.0f, 3.0f });
	}

	int highScore = _FileHandler->ReadOrCreateFile();

	if (highScore != -1)
		pge->DrawStringDecal({ SCREEN_WIDTH * 0.375f, SCREEN_HEIGHT * 0.9f }, "High score: " + std::to_string(highScore), olc::GREY);

	pge->DrawDecal({ 430, 30 }, _Background->decEarth.get(), { 3.0f, 3.0f });
	pge->DrawRotatedDecal({ SCREEN_WIDTH * 0.52f, SCREEN_HEIGHT * 0.55f }, _Player->decPlayer.get(), 0.4f, { 8.0f, 8.0f }, { 7.0f, 7.0f });

	if (pge->GetKey(olc::SPACE).bPressed)
	{
		titleScreen = false;
		_Audio->soundPlayed = false;
		_Audio->PlaySoundSample(pge, 1, 11);
		_Audio->soundPlayed = false;
	}
}

void Interface::LandingMessages(olc::PixelGameEngine* pge, uint16_t totalVelocity, uint16_t gainedScore)
{
	switch (totalVelocity)
	{
	case 0:
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.39f), int(SCREEN_HEIGHT * 0.25f) }, "PERFECT LANDING!");
		break;
	case 1:
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.4f), int(SCREEN_HEIGHT * 0.25f) }, "Great landing!");
		break;
	case 2:
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.4f), int(SCREEN_HEIGHT * 0.25f) }, "Good landing!");
		break;
	case 3:
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.41f), int(SCREEN_HEIGHT * 0.25f) }, "You made it!");
		break;
	case 4:
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.39f), int(SCREEN_HEIGHT * 0.25f) }, "Bit shaky there!");
		break;
	case 5:
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.39f), int(SCREEN_HEIGHT * 0.25f) }, "Almost lost it!");
		break;
	default:
		break;
	}

	pge->DrawStringDecal({ SCREEN_WIDTH * 0.47f, SCREEN_HEIGHT * 0.2f }, "+" + std::to_string(gainedScore));
	pge->DrawStringDecal({ SCREEN_WIDTH * 0.33f, SCREEN_HEIGHT * 0.8f }, "Press SPACE to continue!");
	pge->DrawStringDecal({ SCREEN_WIDTH * 0.43f, SCREEN_HEIGHT * 0.3f }, "+500 fuel", olc::DARK_GREY);
}

void Interface::DeathMessages(olc::PixelGameEngine* pge, FileHandler* _FileHandler, uint16_t totalVelocity, uint32_t currentScore)
{
	uint32_t highScore = _FileHandler->ReadOrCreateFile();

	if (highScore == -1 || highScore < currentScore)
		_FileHandler->OverwriteScore(currentScore);
	else if (highScore == currentScore)
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.4f), int(SCREEN_HEIGHT * 0.3f) }, "NEW HIGH SCORE!");

	if (totalVelocity < 7)
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.31f), int(SCREEN_HEIGHT * 0.25f) }, "You broke the landing gear!");
	else if (totalVelocity < 12)
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.4f), int(SCREEN_HEIGHT * 0.25f) }, "You crashed it!");
	else if (totalVelocity < 20)
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.4f), int(SCREEN_HEIGHT * 0.25f) }, "You wrecked it!");
	else
		pge->DrawStringDecal({ int(SCREEN_WIDTH * 0.37f), int(SCREEN_HEIGHT * 0.25f) }, "YOU BLEW A CRATER!");

	pge->DrawStringDecal({ SCREEN_WIDTH * 0.33f, SCREEN_HEIGHT * 0.8f }, "Press SPACE to restart!");
}

void Interface::FuelGauge(olc::PixelGameEngine* pge, Player* _Player)
{
	float size = ((_Player->fuel / 2250) * 20.0f);

	pge->DrawDecal(      { SCREEN_WIDTH * 0.32f, SCREEN_HEIGHT * 0.952f }, decBar.get(), { size, 0.5f }, olc::GREY);
	pge->DrawStringDecal({ SCREEN_WIDTH * 0.3f,  SCREEN_HEIGHT * 0.95f }, "F");
	pge->DrawStringDecal({ SCREEN_WIDTH * 0.32f + 10.0f * size + 5.0f, SCREEN_HEIGHT * 0.95f }, std::to_string((int)_Player->fuel) + "kg");
}

void Interface::Comms(olc::PixelGameEngine* pge, Player* _Player, float fElapsedTime)
{
	static float   _time = 0.0f;
	static float   randomTime = RandFloat(50.0f, 70.0f);
	static bool    randomVariablesSet = false;
	static bool    isMessageOnScreen = false;
	static uint8_t randomControlQuestion;
	static uint8_t randomCrewReponse;
	static uint8_t randomCrewChatter;
	static uint8_t randomLowAltWarning;
	static uint8_t randomName;

	if (!randomVariablesSet || _Player->dead)
	{
		randomControlQuestion = rand() % 5;
		randomCrewReponse     = rand() % 4;
		randomCrewChatter     = rand() % 3 + 4;
		randomLowAltWarning   = rand() % 3 + 7;
		randomName            = rand() % 3 + 10;
		randomVariablesSet    = true;
	}

	if (!paused)
		_time += fElapsedTime;

	// Allow messsages in ESC-pause, but not landed-pause
	if (_Player->altitude > 0.7f)
	{
		if (_time > randomTime)
		{
			// Command message
			if (_time < randomTime + 8.0f)
				pge->DrawStringDecal({ SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.25f }, "Command:\n" + command[randomControlQuestion], olc::GREY);

			// Crew response
			if (_time > randomTime + 3.0f && _time < randomTime + 8.0f)
				pge->DrawStringDecal({ SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.3f }, "Eagle:\n" + crew[randomCrewReponse], olc::GREY);

			// Reset timers when done
			if (_time > randomTime + 8.0f)
			{
				randomVariablesSet = false;
				randomTime         = RandFloat(50.0f, 70.0f);
				_time              = 0.0f;
			}
		}

		// Crew chatter
		if (_time > randomTime / 2 && _time < randomTime / 2 + 7.0f && _Player->altitude > 4.0f)
			pge->DrawStringDecal({ SCREEN_WIDTH * 0.03f, SCREEN_HEIGHT * 0.35f }, crew[randomName] + crew[randomCrewChatter], olc::GREY, { 0.85f, 0.85f });

		// Low fuel
		if (_Player->fuel < 500)
			pge->DrawStringDecal({ SCREEN_WIDTH * 0.03f, SCREEN_HEIGHT * 0.45f }, crew[10] + crew[13], olc::GREY, { 0.85f, 0.85f });

		// Low alt
		if (_Player->altitude < 4.0f && abs(_Player->currentSegmentAngle) <= 0.349f)
			pge->DrawStringDecal({ SCREEN_WIDTH * 0.03f, SCREEN_HEIGHT * 0.4f }, crew[randomName] + crew[randomLowAltWarning], olc::GREY, { 0.85f, 0.85f });
		else
			randomLowAltWarning = rand() % 3 + 7;
	}
}
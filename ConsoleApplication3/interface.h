#pragma once

#include<string>
#include "player.h"

struct sSegment;
class Background;
class FileHandler;

class Interface
{
private:
	const std::string command[5] = 
	{
		"Eagle, status?", // random questions 0 - 4
		"This is command, status?",
		"How's it going up there?",
		"Eagle, sitrep.",
		"Status report, Eagle.",
	};

	const std::string crew[14] = 
	{
		"This is Eagle, all good.", // responses 0-3
		"We're doing good\n out here!",
		"We could use some beer,\notherwise good!",
		"Command, we're just fine.",
		"Damn, nice view\nout here!", // random 4-6
		"I think I can\nsee my house from here...",
		"Earth is beautiful...",
		"Carefully...", // low alt 7-9
		"Slowly...",
		"Steady...",
		"Capt. James: ", // names 10-12
		"John: ",
		"Fred: ",
		"\nWe're low on fuel!" // fuel
	};

public:
	void Draw(olc::PixelGameEngine* pge, Player* player, float fElapsedTime);
	void TitleScreen(olc::PixelGameEngine* pge, Background* background, Player* player, FileHandler* fileHandler);
	void LandingMessages(olc::PixelGameEngine* pge, sSegment& segment, int vel);
	void DeathMessages(olc::PixelGameEngine* pge, FileHandler* fileHandler, int velocity, int currentScore);
};
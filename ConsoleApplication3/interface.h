#pragma once

#include<string>
#include "olcPixelGameEngine.h"

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
		"We're doing good out here!",
		"We could use some beer,\notherwise good!",
		"Command, we're just fine.",
		"Damn, nice view out here!", // random 4-6
		"I think I can see my\nhouse from here...",
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
	void Draw(olc::PixelGameEngine* pge, float fElapsedTime);
	void TitleScreen(olc::PixelGameEngine* pge);
};
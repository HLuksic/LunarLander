#pragma once

#include<string>
#include "player.h"
#include "audio.h"

struct sSegment;
class Background;
class FileHandler;

class Interface
{
public:
	Interface();

private:
	const std::string command[5];
	const std::string crew[14];

public:
	void Draw(olc::PixelGameEngine* pge, Player* _Player, FileHandler* _FileHandler, float fElapsedTime);
	void TitleScreen(olc::PixelGameEngine* pge, Background* _Background, Player* _Player, FileHandler* _FileHandler, Audio* audio);
	void LandingMessages(olc::PixelGameEngine* pge, int vel, int gainedScore);
	void DeathMessages(olc::PixelGameEngine* pge, FileHandler* _FileHandler, int velocity, int currentScore);
};
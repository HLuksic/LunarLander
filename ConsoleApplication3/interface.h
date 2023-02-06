#pragma once

#include <string>
#include "player.h"
#include "audio.h"

struct Segment;
class Background;
class FileHandler;

class Interface
{
public:
	Interface();

public:
	bool titleScreen;
private:
	const std::string            command[5];
	const std::string            crew[14];
	std::unique_ptr<olc::Sprite> sprBar;
	std::unique_ptr<olc::Decal>  decBar;

public:
	void Draw(olc::PixelGameEngine* pge, Player* _Player, FileHandler* _FileHandler, float fElapsedTime);
	void TitleScreen(olc::PixelGameEngine* pge, Background* _Background, Player* _Player, FileHandler* _FileHandler, Audio* audio);
	void LandingMessages(olc::PixelGameEngine* pge, uint16_t totalVelocity, uint16_t gainedScore);
	void DeathMessages(olc::PixelGameEngine* pge, FileHandler* _FileHandler, uint16_t totalVelocity, uint32_t currentScore);
private:
	void FuelGauge(olc::PixelGameEngine* pge, Player* _Player);
	void Comms(olc::PixelGameEngine* pge, Player* _Player, float fElapsedTime);
};
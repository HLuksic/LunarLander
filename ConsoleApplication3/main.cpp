#include <random>
#include "olcPixelGameEngine.h"
#include "background.h"
#include "interface.h"
#include "player.h"
#include "terrain.h"
#include "filehandler.h"
#include "audio.h"
#include "global.h"

// TODO: 
// FOREGROUND,
// DEBRIS PARTICLES, 
// UI TEXT FIXES

class lunarLander : public olc::PixelGameEngine
{
public:
	lunarLander()
	{
		sAppName = "Lunar lander";
	}

private:
	Player* player;
	Terrain* terrain;
	Interface* userInterface;
	Background* background;
	FileHandler* fileHandler;
	Audio* audio;

public:
	bool OnUserCreate() override
	{
		srand((unsigned int)time(0));

		player        = new Player;
		terrain       = new Terrain;
		userInterface = new Interface;
		background    = new Background;
		fileHandler   = new FileHandler;
		audio         = new Audio;
		paused        = false;
		titleScreen   = true;
		scale         = 0.5f;

		olc::SOUND::InitialiseAudio();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		if (GetKey(olc::ESCAPE).bPressed)
			paused = true;

		if (GetKey(olc::SPACE).bPressed)
			paused = false;

		if (titleScreen)
			userInterface->TitleScreen(this, background, player, fileHandler);
		else
		{
			background->Draw(this, player);
			
			terrain->Spawn(player);
			terrain->Collision(this, player, background, userInterface, fileHandler, audio);
			terrain->Draw(this, player, fElapsedTime);
			
			player->Physics(this, terrain, fElapsedTime);
			player->Draw(this, fElapsedTime);

			userInterface->Draw(this, player, fElapsedTime);

			audio->Play(this, player);
		}
		return true;
	}

	virtual bool OnUserDestroy()
	{
		olc::SOUND::DestroyAudio();
		delete player, terrain, background, userInterface, fileHandler, audio;

		return true;
	}
};

int main() 
{
	lunarLander game;
	
	if (game.Construct(550, 390, 2, 2, false, true))
		game.Start();

	return 0;
}
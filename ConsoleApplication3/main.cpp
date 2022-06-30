#include <random>
#include "olcPixelGameEngine.h"
#include "background.h"
#include "interface.h"
#include "player.h"
#include "terrain.h"
#include "filehandler.h"
#include "global.h"

// TODO: 
// FOREGROUND,
// DEBRIS PARTICLES, 
// SOUNDS,
// HIGH SCORE, 
// UI TEXT FIXES,

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

public:
	bool OnUserCreate() override
	{
		srand((unsigned int)time(0));
		player		= new Player;
		terrain		= new Terrain;
		userInterface	= new Interface;
		background	= new Background;
		fileHandler	= new FileHandler;
		paused		= false;
		titleScreen	= true;
		scale		= 0.5f;

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
			terrain->Collision(this, player, background, userInterface, fileHandler);
			terrain->Draw(this, player);
			
			player->Physics(this, terrain, fElapsedTime);
			player->Draw(this, fElapsedTime);

			userInterface->Draw(this, player, fElapsedTime);
		}
		return true;
	}

	virtual bool OnUserDestroy()
	{
		delete player, terrain, background, userInterface, fileHandler;

		return true;
	}
};

int main() 
{
	lunarLander game;
	
	if (game.Construct(550, 390, 2, 2, true))
		game.Start();

	return 0;
}
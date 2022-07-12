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
// COMMS SOUNDS

class LunarLander : public olc::PixelGameEngine
{
public:
	LunarLander()
	{
		sAppName = "Lunar lander";
	}

private:
	Audio*       _Audio;
	Player*      _Player;
	Terrain*     _Terrain;
	Interface*   _Interface;
	Background*  _Background;
	FileHandler* _FileHandler;

	/*const olc::Pixel backgroundColors[8] = {
		{ olc::BLACK },
		{ olc::VERY_DARK_BLUE },
		{ olc::VERY_DARK_CYAN },
		{ olc::VERY_DARK_GREEN },
		{ olc::VERY_DARK_GREY },
		{ olc::VERY_DARK_MAGENTA },
		{ olc::VERY_DARK_RED },
		{ olc::VERY_DARK_YELLOW },
	};*/

public:
	bool OnUserCreate() override
	{
		srand((unsigned int)time(0));

		_Audio       = new Audio;
		_Player      = new Player;
		_Terrain     = new Terrain;
		_Interface   = new Interface;
		_Background  = new Background;
		_FileHandler = new FileHandler;

		olc::SOUND::InitialiseAudio();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//static uint8_t index = 0;

		Clear(olc::BLACK);

		/*if (GetKey(olc::TAB).bPressed)
		{
			if (index < 7)
				index++;
			else
				index = 0;
		}*/

		if (GetKey(olc::ESCAPE).bPressed)
			Paused = true;

		if (GetKey(olc::SPACE).bPressed)
			Paused = false;

		if (_Interface->titleScreen)
			_Interface->TitleScreen(this, _Background, _Player, _FileHandler, _Audio);
		else
		{
			_Background->Draw(this, _Player);
			
			_Terrain->Spawn(_Player);
			_Terrain->HandleCollision(this, _Player, _Background, _Interface, _FileHandler, _Audio, fElapsedTime);
			_Terrain->Draw(this, _Player, fElapsedTime);
			
			_Player->Physics(this, _Terrain, _Audio, fElapsedTime);
			_Player->Draw(this, fElapsedTime);

			_Interface->Draw(this, _Player, _FileHandler, fElapsedTime);

			_Audio->Play(this, _Player);
		}
		return true;
	}

	virtual bool OnUserDestroy()
	{
		olc::SOUND::DestroyAudio();
		delete _Player, _Terrain, _Background, _Interface, _FileHandler, _Audio;

		return true;
	}
};

int main() 
{
	LunarLander _LunarLander;
	
	if (_LunarLander.Construct(550, 390, 2, 2, false, true))
		_LunarLander.Start();

	return 0;
}
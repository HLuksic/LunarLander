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
// FOREGROUND
// COMMS SOUNDS
// ADD FUEL AND SCORE INCREMENTALLY
// BACKGROUND COLOR SWITCH?

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
		paused       = false;
		scale        = 0.5f;

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
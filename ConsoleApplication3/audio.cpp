#include "audio.h"
#include "global.h"
#include "player.h"

Audio::Audio()
{
	soundSamples[0]  = olc::SOUND::LoadAudioSample("../sfx/engine.wav");
	soundSamples[1]  = olc::SOUND::LoadAudioSample("../sfx/thruster.wav");
	soundSamples[2]  = olc::SOUND::LoadAudioSample("../sfx/launch.wav");
	soundSamples[3]  = olc::SOUND::LoadAudioSample("../sfx/landing1.wav");
	soundSamples[4]  = olc::SOUND::LoadAudioSample("../sfx/landing2.wav");
	soundSamples[5]  = olc::SOUND::LoadAudioSample("../sfx/landing3.wav");
	soundSamples[6]  = olc::SOUND::LoadAudioSample("../sfx/explosion1.wav");
	soundSamples[7]  = olc::SOUND::LoadAudioSample("../sfx/explosion2.wav");
	soundSamples[8]  = olc::SOUND::LoadAudioSample("../sfx/explosion3.wav");
	soundSamples[9]  = olc::SOUND::LoadAudioSample("../sfx/death1.wav");
	soundSamples[10] = olc::SOUND::LoadAudioSample("../sfx/death2.wav");
	soundPlayed      = false;
}


void Audio::Play(olc::PixelGameEngine* pge, Player* player)
{
	if (!player->dead && player->fuel && !paused && pge->GetKey(olc::Key::W).bPressed)
		olc::SOUND::PlaySample(soundSamples[0], true);

	if (!player->dead && player->fuel && !paused && (pge->GetKey(olc::Key::A).bPressed || pge->GetKey(olc::Key::D).bPressed))
		olc::SOUND::PlaySample(soundSamples[1], true);
	
	if (pge->GetKey(olc::Key::W).bReleased)
		olc::SOUND::StopSample(soundSamples[0]);

	if (pge->GetKey(olc::Key::A).bReleased || pge->GetKey(olc::Key::D).bReleased)
		olc::SOUND::StopSample(soundSamples[1]);
}

void Audio::PlayLandingSound(olc::PixelGameEngine* pge)
{
	if (!soundPlayed)
	{
		olc::SOUND::PlaySample(soundSamples[rand() % 3 + 3], false);
		soundPlayed = true;
	}
}

void Audio::PlayDeathSound(olc::PixelGameEngine* pge)
{
	if (!soundPlayed)
	{
		olc::SOUND::PlaySample(soundSamples[rand() % 2 + 9], false);
		soundPlayed = true;
	}
}

void Audio::PlayLaunchSound(olc::PixelGameEngine* pge)
{
	if (!soundPlayed)
		olc::SOUND::PlaySample(soundSamples[2], false);
}

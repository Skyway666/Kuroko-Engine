#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleImGUI.h"
#include "Applog.h"

#pragma comment( lib, "SDL_mixer/lib/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, start_enabled), music(NULL)
{
	name = "audio";
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(JSON_Object* config)
{
	app_log->AddLog("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		app_log->AddLog("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		app_log->AddLog("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		app_log->AddLog("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = true;
	}

	
	LoadFx("Audio/Starting.wav");
	LoadFx("Audio/Motor.wav");
	LoadFx("Audio/Breaks.wav");
	LoadFx("Audio/Crash.wav");

	return ret;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	app_log->AddLog("Freeing sound FX, closing Mixer and Audio subsystem");

	for (std::list<Mix_Music*>::iterator it = music.begin(); it != music.end(); it++)
		Mix_FreeMusic(*it);

	for (std::list<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); it++)
		Mix_FreeChunk(*it);


	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

uint ModuleAudio::LoadMusic(const char* path)
{
	if (Mix_Music* m = Mix_LoadMUS(path))
		music.push_back(m);

	return music.size();
}

bool ModuleAudio::PlayMusic(uint id, float fade_time)
{
	bool ret = true;

	if(Mix_PlayingMusic())
	{
		if(fade_time > 0.0f)
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		else
			Mix_HaltMusic();

	}

	int i = 0;
	for (std::list<Mix_Music*>::iterator it = music.begin(); it != music.end(); it++)
	{
		i++;
		if (id == i)
		{
			if (fade_time > 0.0f)
			{
				if (Mix_FadeInMusic((*it), -1, (int)(fade_time * 1000.0f)) < 0)
				{
					app_log->AddLog("Cannot fade in music. Mix_GetError(): %s", Mix_GetError());
					ret = false;
				}
			}
			else
			{
				if (Mix_PlayMusic((*it), -1) < 0)
				{
					app_log->AddLog("Cannot play in music. Mix_GetError(): %s", Mix_GetError());
					ret = false;
				}
			}
		}
	}

	app_log->AddLog("Successfully playing music");
	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	if (Mix_Chunk* chunk = Mix_LoadWAV(path))
	{
		chunk->volume = ((float)effects_volume / 100.0f) * MIX_MAX_VOLUME;
		fx.push_back(chunk);
	}

	return fx.size();
}

// Play WAV
bool ModuleAudio::PlayFx(uint id, int repeat, int channel)
{
	int i = 0;
	for (std::list<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); it++)
	{
		i++;
		if (id == i)
		{
			Mix_PlayChannel(channel, *it, repeat);
			return true;
		}
	}

	return false;
}


void ModuleAudio::setMasterVolume(uint volume)
{
	master_volume = volume;
	Mix_Volume(-1, ((float)master_volume / 100.0f) * MIX_MAX_VOLUME);
}

void ModuleAudio::setMusicVolume(uint volume)
{
	if (!Mix_FadingMusic())
	{
		music_volume = volume;
		Mix_VolumeMusic(((float)effects_volume / 100.0f) * MIX_MAX_VOLUME);
	}
}

void ModuleAudio::setEffectsVolume(uint volume)
{
	effects_volume = volume;
	for (std::list<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); it++)
		(*it)->volume = ((float)effects_volume / 100.0f) * MIX_MAX_VOLUME;
}
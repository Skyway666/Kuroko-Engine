#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleImGUI.h"
#include "Applog.h"


ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "audio";
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(JSON_Object* config)
{
	//app_log->AddLog("Loading Audio Mixer");
	//bool ret = true;
	//SDL_Init(0);

	//if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	//{
	//	app_log->AddLog("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
	//	ret = false;
	//}

	//// load support for the OGG format
	//int flags = MIX_INIT_OGG;
	//int init = Mix_Init(flags);

	//if((init & flags) != flags)
	//{
	//	app_log->AddLog("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
	//	ret = true;
	//}

	////Initialize SDL_mixer
	//if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	//{
	//	app_log->AddLog("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	//	ret = true;
	//}

	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	/*app_log->AddLog("Freeing sound FX, closing Mixer and Audio subsystem");

	audio_files.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);*/
	return true;
}

AudioFile* ModuleAudio::LoadAudio(const char* path, const char* name, AudioType type)
{/*
	AudioFile* audio_file = nullptr;
	void* data = nullptr;

	if (type == FX)
		data = (void*)Mix_LoadWAV(path);
	else if (type == MUSIC)
		data = (void*)Mix_LoadMUS(path);

	if (data)
	{
		audio_file = new AudioFile(name, last_audio_id++, data, type);
		audio_files.push_back(audio_file);
	}

	return audio_file;*/
	return nullptr;
}


void ModuleAudio::Play(const char* name, uint repeat)
{/*
	for (std::list<AudioFile*>::iterator it = audio_files.begin(); it != audio_files.end(); it++)
		if ((*it)->name == name)
		{
			(*it)->Play(repeat);
			return;
		}
		*/
}

void ModuleAudio::Play(uint id, uint repeat)
{
	/*for (std::list<AudioFile*>::iterator it = audio_files.begin(); it != audio_files.end(); it++)
		if ((*it)->id == id)
		{
			(*it)->Play(repeat);
			return;
		}
*/
}

void ModuleAudio::HaltMusic()
{
	/*Mix_HaltMusic();*/
}

void ModuleAudio::setMasterVolume(uint volume)
{/*
	master_volume = volume;
	Mix_Volume(-1, master_volume * MIX_MAX_VOLUME);*/
}

void ModuleAudio::setMasterMusicVolume(uint volume)
{
	//if (!Mix_FadingMusic())
	//{
	//	music_volume = volume;
	//	Mix_VolumeMusic(music_volume  * MIX_MAX_VOLUME);
	//}
}

AudioFile::~AudioFile()
{
	//if (data)
	//{
	//	if(type == MUSIC)	Mix_FreeMusic((Mix_Music*)data);
	//	else				Mix_FreeChunk((Mix_Chunk*)data);
	//}
}

void AudioFile::Play(uint repeat)
{
//	if (data)
//	{
//		if(type == FX)					// for FX
//			channel = Mix_PlayChannel(-1, (Mix_Chunk*)data, repeat);  
//		else if (type == MUSIC)			// for music
//		{
//			if (Mix_PlayingMusic())
//			{
//				if (fade_time > 0.0f)	Mix_FadeOutMusic((int)(fade_time * 1000.0f));
//				else					Mix_HaltMusic();
//			}
//
//			int res = -1;
//			if (fade_time > 0.0f)	res = Mix_FadeInMusic((Mix_Music*)data, -1, (int)(fade_time * 1000.0f));
//			else					res = Mix_PlayMusic((Mix_Music*)data, -1);
//			
//			if(res < 0)
//				app_log->AddLog("Cannot fade in music. Mix_GetError(): %s", Mix_GetError());
//		}
//	}
}

void AudioFile::Stop()
{
//	if (data)
//	{
//		if (type == FX)					// for FX
//			Mix_HaltChannel(channel);
//		else if (type == MUSIC)			// for music
//			Mix_HaltMusic();
//	}
}
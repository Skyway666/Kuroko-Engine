#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include <list>

#define DEFAULT_MUSIC_FADE_TIME 2.0f

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool Init(JSON_Object* config);
	bool CleanUp();

	unsigned int LoadMusic(const char* path);
	bool PlayMusic(uint id, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	unsigned int LoadFx(const char* path);
	bool PlayFx(uint id, int repeat = 0, int channel = -1);

	void setMasterVolume(uint volume);
	void setMusicVolume(uint volume);
	void setEffectsVolume(uint volume);
	uint getMasterVolume()	{ return master_volume; };
	uint getMusicVolume()	{ return music_volume; };
	uint getEffectsVolume() { return effects_volume; };

private:

	std::list<Mix_Music*>	music;
	std::list<Mix_Chunk*>	fx;

	uint master_volume	= 100;	 // _seriazible_var
	uint music_volume	= 100;	 // _seriazible_var
	uint effects_volume = 100;   // _seriazible_var
};

#endif // __ModuleAudio_H__
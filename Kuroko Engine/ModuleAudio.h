#ifndef _MODULE_AUDIO_H_
#define _MODULE_AUDIO_H_

#include "Module.h"
//#include "Include_Wwise.h"
#include "Wwise.h"


#define AUDIO_DIRECTORY "Library/Sounds/"
#define AUDIO_EXTENSION ".bnk"
#define DEFAULT_VOLUME 50

//class SoundBank;

class ModuleAudio : public Module {

public:

	ModuleAudio(bool start_enabled = true);
	virtual ~ModuleAudio();
	bool Init(Document& document) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	bool Save(Document& document, FileWriteStream& fws)const override;
	bool Load(Document& document) override;

	void LoadSoundBank(const char* path);
	
	void SetVolume(const char* rtpc, float value);

public:
	int volume = DEFAULT_VOLUME;
	bool muted = false;
	
};


#endif // _MODULE_AUDIO_H_
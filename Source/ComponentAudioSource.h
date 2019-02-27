#ifndef _COMPONENT_AUDIO_SOURCE_H_
#define _COMPONENT_AUDIO_SOURCE_H_

#include "Component.h"
#include "Wwise.h"
//#include "Game\Assets\Sounds\Wwise_IDs.h"

class WwiseGameObject;

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(GameObject* parent);
	ComponentAudioSource(JSON_Object* deff, GameObject* parent);
	~ComponentAudioSource();

	bool Update(float dt) override;
	void CleanUp();
	//void SetInspectorInfo() override;

	void Save(JSON_Object* config) override;

	void SetSoundID(AkUniqueID ID);
	void SetSoundName(const char* newName);

public:
	Wwise::WwiseGameObject* sound_go = nullptr;
	AkUniqueID sound_ID = 0;
	std::string name = "Sound";
	int volume = 100;
	int pitch = 0;
};

#endif
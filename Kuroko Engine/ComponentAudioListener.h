#ifndef _COMPONENT_AUDIO_LISTENER_H_
#define _COMPONENT_AUDIO_LISTENER_H_

#include "Component.h"
#include "Wwise.h"


class WwiseGameObject;

class ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(GameObject* gameobject);
	~ComponentAudioListener();

	bool Update(float dt) override;
	/*void SetInspectorInfo() override;

	Value Save(Document::AllocatorType& allocator) const override;
	bool Load(Document& document) override;*/

public:
	Wwise::WwiseGameObject* sound_go = nullptr;

};

#endif
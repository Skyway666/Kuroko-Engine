#include "ComponentAudioSource.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Transform.h"


ComponentAudioSource::ComponentAudioSource(GameObject* parent) : Component(parent, AUDIOSOURCE)
{
	float3 pos = ((ComponentTransform*)parent->getComponent(Component_type::TRANSFORM))->local->getPosition();
	sound_go = Wwise::CreateSoundObj(parent->getUUID(), parent->getName().c_str(), pos.x, pos.y, pos.z);
}

ComponentAudioSource::ComponentAudioSource(JSON_Object* deff, GameObject* parent) : Component(parent, AUDIOSOURCE)
{
	float3 pos = ((ComponentTransform*)parent->getComponent(Component_type::TRANSFORM))->local->getPosition();
	sound_go = Wwise::CreateSoundObj(parent->getUUID(), parent->getName().c_str(), pos.x, pos.y, pos.z);

	is_active = json_object_get_boolean(deff, "active");
	sound_ID = json_object_get_number(deff, "soundID");
}

ComponentAudioSource::~ComponentAudioSource()
{
}


bool ComponentAudioSource::Update(float dt)
{
	float3 pos = ((ComponentTransform*)parent->getComponent(Component_type::TRANSFORM))->local->getPosition();
	sound_go->SetPosition(pos.x, pos.y, pos.z);

	return true;
}

//void ComponentAudioSource::SetInspectorInfo()
//{
//	ImGui::Spacing();
//	if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		if (sound_ID != 0)
//		{
//			if (ImGui::Button("Play"))
//			{
//				sound_go->PlayEvent(sound_ID);
//			}
//			if (ImGui::Button("Stop"))
//			{
//				sound_go->PauseEvent(sound_ID);
//			}
//		}
//		else
//		{
//			ImGui::TextColored({ 1, 0, 0, 1 }, "No Audio Event assigned!");
//		}
//		/*ImGui::Text("PosX %f", sound_go->GetPos().x);
//		ImGui::Text("PosY %f", sound_go->GetPos().y);
//		ImGui::Text("PosZ %f", sound_go->GetPos().z);*/
//	}
//}

void ComponentAudioSource::Save(JSON_Object* config)
{
	json_object_set_string(config, "type", "audioSource");
	json_object_set_boolean(config, "active", is_active);
	json_object_set_number(config, "soundID", sound_ID);
}

void ComponentAudioSource::SetSoundID(AkUniqueID ID)
{
	sound_ID = ID;
}
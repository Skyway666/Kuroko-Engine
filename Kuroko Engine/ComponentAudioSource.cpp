#include "ComponentAudioSource.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "mmgr/mmgr.h"


ComponentAudioSource::ComponentAudioSource(GameObject* gameobject)
{
	my_go = gameobject;
	type = AUDIOSOURCE;
	UUID = pcg32_random_r(&App->rng);

	float3 pos = my_go->GetComponent(Component::TRANSFORMATION)->AsTransform()->position;
	sound_go = Wwise::CreateSoundObj(my_go->UUID, my_go->go_name.c_str(), pos.x, pos.y, pos.z);
}

ComponentAudioSource::~ComponentAudioSource()
{
}


bool ComponentAudioSource::Update()
{
	float3 pos = my_go->GetComponent(Component::TRANSFORMATION)->AsTransform()->position;
	sound_go->SetPosition(pos.x, pos.y, pos.z);

	return true;
}

void ComponentAudioSource::SetInspectorInfo()
{
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (sound_ID != 0)
		{
			if (ImGui::Button("Play"))
			{
				sound_go->PlayEvent(sound_ID);
			}
			if (ImGui::Button("Stop"))
			{
				sound_go->PauseEvent(sound_ID);
			}
		}
		else
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "No Audio Event assigned!");
		}
		/*ImGui::Text("PosX %f", sound_go->GetPos().x);
		ImGui::Text("PosY %f", sound_go->GetPos().y);
		ImGui::Text("PosZ %f", sound_go->GetPos().z);*/
	}
}

Value ComponentAudioSource::Save(Document::AllocatorType& allocator) const
{
	Value CompArray(kObjectType);

	return CompArray;
}

bool ComponentAudioSource::Load(Document& document)
{
	return true;
}

void ComponentAudioSource::SetSoundID(AkUniqueID ID)
{
	sound_ID = ID;
}
#include "ComponentAudioListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "mmgr/mmgr.h"


ComponentAudioListener::ComponentAudioListener(GameObject* gameobject)
{
	my_go = gameobject;
	type = AUDIOLISTENER;
	UUID = pcg32_random_r(&App->rng);

	float3 pos = my_go->GetComponent(Component::TRANSFORMATION)->AsTransform()->position;
	sound_go = Wwise::CreateSoundObj(my_go->UUID, my_go->go_name.c_str(), pos.x, pos.y, pos.z, true);
}

ComponentAudioListener::~ComponentAudioListener()
{
}


bool ComponentAudioListener::Update()
{
	float3 pos = App->scene->GetCurCam()->GetFrustum().pos;
	float3 front = App->scene->GetCurCam()->GetFrustum().front;
	float3 top = App->scene->GetCurCam()->GetFrustum().up;
	sound_go->SetPosition(pos.x, pos.y, pos.z, front.x, front.y, front.z, top.x, top.y, top.z);

	return true;
}

void ComponentAudioListener::SetInspectorInfo()
{
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Audio Listener", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Mute", &App->audio->muted);
		if (ImGui::InputInt("Volume", &App->audio->volume))
		{
			App->audio->SetVolume("Volume", App->audio->volume);
			App->audio->muted = false;
		}
		/*ImGui::Text("PosX %f", sound_go->GetPos().x);
		ImGui::Text("PosY %f", sound_go->GetPos().y);
		ImGui::Text("PosZ %f", sound_go->GetPos().z);*/
	}
}

Value ComponentAudioListener::Save(Document::AllocatorType& allocator) const
{
	Value CompArray(kObjectType);

	return CompArray;
}

bool ComponentAudioListener::Load(Document& document)
{
	return true;
}
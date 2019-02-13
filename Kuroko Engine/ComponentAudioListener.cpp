#include "ComponentAudioListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "Camera.h"


ComponentAudioListener::ComponentAudioListener(GameObject* gameobject) : Component(gameobject, AUDIOLISTENER)
{
	float3 pos = ((ComponentTransform*)parent->getComponent(Component_type::TRANSFORM))->local->getPosition();
	sound_go = Wwise::CreateSoundObj(parent->getUUID(), parent->getName().c_str(), pos.x, pos.y, pos.z, true);
}

ComponentAudioListener::~ComponentAudioListener()
{
}


bool ComponentAudioListener::Update(float dt)
{
	float3 pos = App->camera->current_camera->getFrustum()->pos;
	float3 front = App->camera->current_camera->getFrustum()->front;
	float3 top = App->camera->current_camera->getFrustum()->up;
	sound_go->SetPosition(pos.x, pos.y, pos.z, front.x, front.y, front.z, top.x, top.y, top.z);

	return true;
}

//void ComponentAudioListener::SetInspectorInfo()
//{
//	ImGui::Spacing();
//	if (ImGui::CollapsingHeader("Audio Listener", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		ImGui::Checkbox("Mute", &App->audio->muted);
//		if (ImGui::InputInt("Volume", &App->audio->volume))
//		{
//			App->audio->SetVolume("Volume", App->audio->volume);
//			App->audio->muted = false;
//		}
//		/*ImGui::Text("PosX %f", sound_go->GetPos().x);
//		ImGui::Text("PosY %f", sound_go->GetPos().y);
//		ImGui::Text("PosZ %f", sound_go->GetPos().z);*/
//	}
//}
//
//Value ComponentAudioListener::Save(Document::AllocatorType& allocator) const
//{
//	Value CompArray(kObjectType);
//
//	return CompArray;
//}
//
//bool ComponentAudioListener::Load(Document& document)
//{
//	return true;
//}
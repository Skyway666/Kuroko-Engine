#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleUi.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentAudioListener.h"
#include "Wwise.h"

#include <corecrt_wstring.h>

#include "Wwise/IO/Win32/AkFilePackageLowLevelIOBlocking.h"


//ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled)
//{
//	//name
//}
//
//ModuleAudio::~ModuleAudio()
//{
//
//}
//
//bool ModuleAudio::Init(Document& document)
//{
//	LOG("Initializing Wwise");
//	
//	bool ret = Wwise::InitWwise();
//	
//	LoadSoundBank("Music");
//	
//	return ret;
//}
//
//bool ModuleAudio::Start()
//{
//	return true;
//}
//
//update_status ModuleAudio::PreUpdate(float dt)
//{
//	if (!muted) {
//		SetVolume("Volume", volume);
//	}
//	else {
//		SetVolume("Volume", 0);
//	}
//
//	float3 cam_pos = App->scene->GetCurCam()->transform.position;
//	App->scene->audiolistenerdefault->GetComponent(Component::TRANSFORMATION)->AsTransform()->position = cam_pos;
//	App->scene->audiolistenerdefault->GetComponent(Component::TRANSFORMATION)->AsTransform()->CalculateMatrix();
//
//	return UPDATE_CONTINUE;
//}
//
//
//update_status ModuleAudio::PostUpdate(float dt)
//{
//
//	AK::SoundEngine::RenderAudio();
//
//	return UPDATE_CONTINUE;
//}
//
//bool ModuleAudio::CleanUp()
//{
//	LOG("Freeing Wwise");
//	
//	return Wwise::CloseWwise();
//
//	return true;
//}
//
//bool ModuleAudio::Save(Document& document, FileWriteStream& fws)const
//{
//	bool ret = true;
//
//	return ret;
//}
//
//bool ModuleAudio::Load(Document& document)
//{
//	bool ret = true;
//
//	return ret;
//}
//
//// -----------------------------
//
//void ModuleAudio::SetVolume(const char* rtpcID, float value)
//{
//	AKRESULT eResult = AK::SoundEngine::SetRTPCValue(rtpcID, value, App->scene->audiolistenerdefault->GetComponent(Component::AUDIOLISTENER)->AsAudioListener()->sound_go->GetID());
//	if (eResult != AK_Success)
//	{
//		assert(!"Error changing audio volume!");
//	}
//}
//
//void ModuleAudio::LoadSoundBank(const char* path)
//{
//	//SoundBank* new_bank = new SoundBank();
//	std::string bank_path = AUDIO_DIRECTORY;
//	bank_path += path;
//	bank_path += AUDIO_EXTENSION;
//
//	Wwise::LoadBank(bank_path.c_str());
//
//	std::string json_file = bank_path.substr(0, bank_path.find_last_of('.')) + ".json"; // Changing .bnk with .json
//	/*GetBankInfo(json_file, new_bank);
//	soundbanks.push_back(new_bank);
//	soundbank = new_bank;
//	return new_bank;*/
//}

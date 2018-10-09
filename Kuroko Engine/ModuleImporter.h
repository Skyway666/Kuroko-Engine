#pragma once
#include "Module.h"
#include "Material.h"
#include <vector>

class GameObject;
class ComponentMesh;
class aiNode;
class aiScene;
class AudioFile;

enum ImportType { I_NONE, I_GOBJ, I_TEXTURE, I_FX, I_MUSIC};

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init(JSON_Object* config);
	bool CleanUp();

	void* Import(const char* file, ImportType expected_filetype = I_NONE);

private:
	bool LoadMaterials(const aiScene* scene, std::vector<uint>& out_mat_id);
	GameObject* LoadMeshRecursive(aiNode* node, const aiScene* scene, const std::vector<uint>& in_mat_id, GameObject* parent = nullptr);


};
#pragma once
#include "Module.h"
#include "Material.h"
#include <vector>


class GameObject;
class ComponentMesh;
class aiNode;
class aiScene;


class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init(JSON_Object* config);
	bool CleanUp();

	GameObject* LoadFBX(const char* file);
	Texture* LoadTex(char* file, Mat_Wrap wrap = CLAMP, Mat_MinMagFilter min_filter = LINEAR, Mat_MinMagFilter mag_filter = LINEAR);

private:
	uint LoadMaterials(const aiScene* scene, std::vector<uint>& out_mat_id);
	GameObject* LoadMeshRecursive(aiNode* node, const aiScene* scene, const std::vector<uint>& in_mat_id, GameObject* parent = nullptr);
	

};
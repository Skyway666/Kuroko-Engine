#pragma once
#include "Module.h"
#include "Material.h"

class GameObject;
class ComponentMesh;
class aiNode;
class aiScene;
class Material;

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init();
	bool CleanUp();

	GameObject* LoadFBX(const char* file);
	GameObject* LoadAssimpNode(aiNode* node, const aiScene* scene, GameObject* parent = nullptr);
	bool LoadRootMesh(const char* file, ComponentMesh* component_to_load);	 // this method will only load the root mesh of an FBX, if existent. To load a full FBX scene, use LoadFBX()
	Material* quickLoadTex(char* file);
	Material* LoadTex(char* file, Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter);
};
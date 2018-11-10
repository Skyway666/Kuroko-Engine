#ifndef __MODULE_IMPORTER
#define __MODULE_IMPORTER
#include "Module.h"
#include "Material.h"
#include <vector>

class GameObject;
class ComponentMesh;
class aiNode;
class aiScene;
class AudioFile;
class Mesh;

enum ImportType { I_NONE, I_GOBJ, I_TEXTURE, I_FX, I_MUSIC};

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init(const JSON_Object* config);
	bool CleanUp();

	void* Import(const char* file, ImportType expected_filetype = I_NONE);

	// This functions will eventually replace "Import", when resource manager is working
	bool ImportTexture(const char* file_original_name, std::string file_binary_name); // Retruns true on successful import and false if it wasn't
	bool ImportScene(const char* file_original_name, std::string file_binary_name);	  // Not gonna handle materials for now, only meshes.
	//Own file format
	void ExportMeshToKR(const char* file, Mesh* mesh);
	void ExportTextureToDDS(const char* file);
	Mesh* ImportMeshFromKR(const char* file); // Code crashes when importing mesh from .kr without importing something first

private:
	void LoadMaterials(const aiScene& scene, std::vector<uint>& out_mat_id) const;
	GameObject* LoadNodeRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent = nullptr);
	void LoadNodeToSceneRecursive(const aiNode& node, const aiScene& scene, JSON_Value* json_scene);

};
void logAssimp(const char* message, char* user);
#endif
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
	//Own file format
	void ExportMeshToKR(const char* file, Mesh* mesh);
	void ExportTextureToDDS(const char* file);
	Mesh* ImportMeshFromKR(const char* file); // Code crashes when importing mesh from .kr without importing something first

private:
	void LoadMaterials(const aiScene& scene, std::vector<uint>& out_mat_id) const;
	GameObject* LoadNodeRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent = nullptr);

};
void logAssimp(const char* message, char* user);
#endif
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

struct material_resource_deff {
	std::string binary_path_diffuse;
	std::string binary_path_ambient;
	std::string binary_path_normals;
	std::string binary_path_lightmap;
};

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
	Texture* LoadTextureFromLibrary(const char* file);

private:
	void LoadMaterials(const aiScene& scene, std::vector<uint>& out_mat_id) const;
	GameObject* LoadNodeRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent = nullptr);
	void ImportNodeToSceneRecursive(const aiNode& node, const aiScene& scene, JSON_Value* json_scene, const std::vector<material_resource_deff>& in_mat_id, uint parent = -1);  // TODO: Add a parameter for mat id, a vector of JSON_Objects*
	void ImportMaterialsFromNode(const aiScene& scene, std::vector<material_resource_deff>& out_mat_id);

};
void logAssimp(const char* message, char* user);
#endif
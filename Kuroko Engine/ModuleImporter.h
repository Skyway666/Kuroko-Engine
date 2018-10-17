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

	bool Init(const JSON_Object& config);
	bool CleanUp();


	void* Import(const char* file, ImportType expected_filetype = I_NONE);
	//Own file format
	void ExportMeshToKR(const char* file, Mesh* mesh);
	void ExportTextureToKR(const char* file, Texture* tex);


	// Useful functions to work with paths
	bool removeExtension(std::string& str);
	bool removePath(std::string& str);
	void getFileNameFromPath(std::string& str);

private:
	void LoadMaterials(const aiScene& scene, std::vector<uint>& out_mat_id) const;
	GameObject* LoadMeshRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent = nullptr) const;


};
void logAssimp(const char* message, char* user);
#endif
#ifndef __MODULE_RESOURCES__
#define __MODULE_RESOURCES__

#include "Module.h"
#include "Resource.h"
#include <map>
enum lib_dir;
class ModuleResourcesManager: public Module
{
public:
	ModuleResourcesManager(Application* app, bool start_enabled = true);
	~ModuleResourcesManager();

	bool Init(const JSON_Object* config); // Set values from config
	bool Start();						  // Export everything on the assets folder if it has no metadata
	// Check assets library and test all the assets against their .meta files every 3 seconds
	// If:
	// 1. Metadata has no asset -> Erase metadata and file from library
	// 2. Asset has no metadata -> Generate metadata and import it to library
	// 3. Asset and metadata have different "last edited time" values -> Destroy old file in library, reimport file to library and change metadata
	update_status Update(float dt);		
	bool CleanUp();						  // Unload all the ocupied resources


	// To be executed allways when engine starts. 
	// Fills resources list for each file in assets, and the ones which don't have .meta are exported to library and given.meta
	void GenerateLibraryAndMeta();
	void ManageMeta(std::string path, std::string name, std::string extension);
	void CleanMeshesFromLibrary(std::string prefab_binary);
	void ManageAsset(std::string path, std::string name, std::string extension);
	// Generates Imports file to library, with the name of the uuid
	// IMPORTANT: If the resource is an FBX, it exports all the meshes as different binarys, and only one for the FBX which is a scene,
	// with the transformations and hierarchy, and uuids of the meshes that it contains. Only one .metadata is generated, and it points
	// to that file.
	void ImportToLibrary(const char* file_original_name, std::string file_binary_name);				

	Resource* getResource(uint uuid);
	// Creates a resource from a .meta that contains uuid, type, extension, timeCreated...
	// IMPORTANT: Resources for meshes will be created by the constructor of the scene resource, as it can access
	// binarys, original asset, uuid, and type, of course.
	Resource* newResource(resource_deff deff);		
	void LoadResource(uint uuid);						// Iterates resource list, looks for the resource and allows it to load


	// Looks for the file's metadata. If it doesn't find it, it calls ImportToLibrary(const char* file)
	// When it has it, looks for the resource, and tells it to LoadToMemory(). The only type of asset that
	// will generate objects in the scene is FBX, as it is a scene itself. The other ones will just be loaded to memory,
	// and showcased in the resources debug UI
	void LoadFileToScene(const char* file);		



private:
	std::map<uint, Resource*> resources;		

	void CleanMeta();
	// Helpers
	std::string uuid2string(uint uuid);			// Converts a uuid into a file name, to be able to read from the library
	const char* assetExtension2type(const char* extension); // Converts an extension into a resource file
	ResourceType type2enumType(const char* type);
	const char* enumType2binaryExtension(ResourceType type);
	lib_dir enumType2libDir(ResourceType type);

};

#endif
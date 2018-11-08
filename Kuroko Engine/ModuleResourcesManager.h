#ifndef __MODULE_RESOURCES__
#define __MODULE_RESOURCES__

#include "Module.h"
#include <list>

class Resource {

public:
	uint uuid;
	void LoadToMemory();		// Load resource to memory
	void UnloadFromMemory();    // Unload resource from memory (should put resource pointer to null)
	bool IsLoaded();			// If the resource is loaded to memory

protected:

	uint components_used_by = 0; // How many components use this resource


};

class ModuleResourcesManager: public Module
{
public:
	ModuleResourcesManager(Application* app, bool start_enabled = true);
	~ModuleResourcesManager();

	bool Init(const JSON_Object* config); // Set values from config
	bool Start();						  // Export everything on the assets folder if it has no metadata
	//update_status PreUpdate(float dt);
	update_status Update(float dt);
	//update_status PostUpdate(float dt);
	bool CleanUp();						  // Unload all the ocupied resources


	Resource* ExportToLibrary(const char* file); // Generates .meta and imports file to library. Returns UID of the given resource
	void GenerateLibraryFromAssets();		// To be executed allways when engine starts. 
											// Fills resources list for each file in assets, and the ones which don't have .meta are exported to library and given.meta

	std::list<Resource*> resources;
										  
};

#endif
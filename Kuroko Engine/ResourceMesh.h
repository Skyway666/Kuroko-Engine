#pragma once
#include "Resource.h"
class Mesh;
class ResourceMesh :
	public Resource {
public:
	ResourceMesh(resource_deff deff);
	~ResourceMesh();
	void LoadToMemory();
	void UnloadFromMemory();
public:
	Mesh* mesh;
};


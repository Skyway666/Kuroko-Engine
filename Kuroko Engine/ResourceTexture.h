#pragma once
#include "Resource.h"

struct Texture;
class ResourceTexture :
	public Resource {
public:
	ResourceTexture(resource_deff deff);
	~ResourceTexture();
	void LoadToMemory();
	void UnloadFromMemory();
public:
	Texture* texture;
	bool drawn_in_UI = false;
};


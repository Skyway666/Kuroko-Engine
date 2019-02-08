#ifndef __RESOURCEBONE_H__
#define __RESOURCEBONE_H__

#include "Resource.h"

class ResourceBone :
	public Resource
{
public:

	ResourceBone(resource_deff deff);
	~ResourceBone();

	void LoadToMemory();
	void UnloadFromMemory();
};

#endif // !__RESOURCEBONE_H__

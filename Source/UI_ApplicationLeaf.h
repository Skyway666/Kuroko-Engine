#ifndef __UI_APPLICATION_LEAF_
#define __UI_APPLICATION_LEAF_

#include "UI_Leaf.h"

class UI_ApplicationLeaf : public UI_Leaf
{
public:
	UI_ApplicationLeaf() : UI_Leaf("Application", UI_APPLICATION) {};
	void UpdateContent();

private:

};


#endif
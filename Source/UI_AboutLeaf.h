#ifndef __UI_ABOUT_LEAF_
#define __UI_ABOUT_LEAF_

#include "UI_Leaf.h"

class UI_AboutLeaf : public UI_Leaf
{
public:
	UI_AboutLeaf() : UI_Leaf("About", UI_ABOUT) {};
	void UpdateContent();

private:

};


#endif
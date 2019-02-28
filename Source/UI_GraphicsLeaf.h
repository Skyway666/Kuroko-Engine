#pragma once
#ifndef __UI_GRAPHICS_LEAF_
#define __UI_GRAPHICS_LEAF_

#include "UI_Leaf.h"

class UI_GraphicsLeaf : public UI_Leaf
{
public:
	UI_GraphicsLeaf() : UI_Leaf("Graphics", UI_PRIMITIVE) {};
	void UpdateContent();

private:

};


#endif
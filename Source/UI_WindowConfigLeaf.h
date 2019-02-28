#pragma once
#ifndef __UI_WINDOWCONFIG_LEAF_
#define __UI_WINDOWCONFIG_LEAF_

#include "UI_Leaf.h"

class UI_WindowConfigLeaf : public UI_Leaf
{
public:
	UI_WindowConfigLeaf() : UI_Leaf("Window config.", UI_WINDOW_CONFIG) {};
	void UpdateContent();

private:

};


#endif
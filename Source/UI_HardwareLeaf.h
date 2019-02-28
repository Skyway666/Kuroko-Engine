#ifndef __UI_HARDWARE_LEAF_
#define __UI_HARDWARE_LEAF_

#include "UI_Leaf.h"

class UI_HardwareLeaf : public UI_Leaf
{
public:
	UI_HardwareLeaf() : UI_Leaf("Hardware", UI_HARDWARE) {};
	void UpdateContent();

private:

};


#endif
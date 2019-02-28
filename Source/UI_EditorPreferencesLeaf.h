#ifndef __UI_EDITORPREFERENCES_LEAF_
#define __UI_EDITORPREFERENCES_LEAF_

#include "UI_Leaf.h"

class UI_EditorPreferencesLeaf : public UI_Leaf
{
public:
	UI_EditorPreferencesLeaf() : UI_Leaf("About", UI_ABOUT) {};
	void UpdateContent();

private:

};


#endif
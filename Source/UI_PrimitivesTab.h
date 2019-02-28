#ifndef __UI_PRIMITIVES_TAB_
#define __UI_PRIMITIVES_TAB_

#include "UI_Window.h"

class UI_PrimitivesTab : public UI_Tab
{
public:
	UI_PrimitivesTab() : UI_Tab("Primitives", UI_PRIMITIVE) {};
	void UpdateContent();

private:

};


#endif
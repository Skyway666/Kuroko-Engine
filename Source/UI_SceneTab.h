#ifndef __UI_OBJECT_INSPECTOR_TAB_
#define __UI_OBJECT_INSPECTOR_TAB_

#include "UI_Window.h"
#include "MathGeoLib/Math/float2.h"

class GameObject;

class UI_SceneTab : public UI_Tab
{
public:
	UI_SceneTab() : UI_Tab("Scene", UI_SCENE) {};
	void UpdateContent();

public:
	ImTextureID img;

private:
	float2 lastSize;
	float2 size;
	float2 pos;
	ImVec2 region;
};


#endif
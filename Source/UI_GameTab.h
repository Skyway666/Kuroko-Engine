#ifndef __UI_GAME_TAB_
#define __UI_GAME_TAB_

#include "UI_Window.h"
#include "MathGeoLib/Math/float2.h"

class GameObject;

class UI_GameTab : public UI_Tab
{
public:
	UI_GameTab() : UI_Tab("Game", UI_GAME) {};
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
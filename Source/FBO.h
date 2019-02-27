#ifndef __FBO_H__
#define __FBO_H__

#include "Globals.h"

//TUTORIAL USED: https://gamedev.stackexchange.com/questions/140693/how-can-i-render-an-opengl-scene-into-an-imgui-window

//FBO = Frame Buffer Object.It is used for the creation of textures to send to the imgui image

class FBO
{
public:
	FBO();
	~FBO();

	void Create(int width, int height);

	void BindFBO();
	void UnBindFBO();

	void DeleteFBO();

public:
	int texture;
	int framebuffer;
	int rbo;
	int width;
	int height;
};

#endif //!__FBO_H__
#pragma once
#include "Globals.h"

enum Mat_Wrap { W_UNKNOWN, CLAMP, CLAMP_TO_BORDER, REPEAT, MIRRORED_REPEAT };
enum Mat_MinMagFilter { M_UNKNOWN, NEAREST, LINEAR, MIPMAP_NEAREST, MIPMAP_LINEAR};
enum TextureType { DIFFUSE, AMBIENT, NORMALS, LIGHTMAP};

#define SIZE_CHECKERED 64

struct Texture
{
public:
	Texture() {};
	Texture(uint GL_id);
	~Texture();

	uint getGLid() { return gl_id; };
	void getSize(int& x, int& y) { x = size_x; y = size_y; };

	void setParameters(Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter);
	void LoadCheckered();

public:
	uint gl_id = 0;

	Mat_Wrap wrap_mode = W_UNKNOWN;
	Mat_MinMagFilter min_filter_mode = M_UNKNOWN;
	Mat_MinMagFilter mag_filter_mode = M_UNKNOWN;

private:
	int size_x = 0;
	int size_y = 0;
};

class Material
{
public:

	Material();
	~Material();

	Texture* getTexture(TextureType tex_type);
	void setTexture(TextureType tex_type, Texture* texture);
	void setCheckeredTexture(TextureType tex_type = DIFFUSE);
	uint getId() { return id; };

private:
	uint id = 0;
	Texture* diffuse = nullptr;
	Texture* ambient = nullptr;
	Texture* normals = nullptr;
	Texture* lightmap = nullptr;

};
#pragma once
#include "Globals.h"

enum Mat_Wrap { W_UNKNOWN, CLAMP, CLAMP_TO_BORDER, REPEAT, MIRRORED_REPEAT };
enum Mat_MinMagFilter { M_UNKNOWN, NEAREST, LINEAR, MIPMAP_NEAREST, MIPMAP_LINEAR};

#define SIZE_CHECKERED 64

class Material
{
public:

	Material() {};
	Material(uint GL_id);
	~Material();

	void setParameters(Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter);
	void LoadCheckered();
	bool isLoaded() { return loaded; };
	uint getGLid() { return id; };
	void getSize(int& x, int& y) { x = size_x; y = size_y; };

private:
	uint id = 0;
	int size_x = 0;
	int size_y = 0;

	Mat_Wrap wrap_mode = W_UNKNOWN;
	Mat_MinMagFilter min_filter_mode = M_UNKNOWN;
	Mat_MinMagFilter mag_filter_mode = M_UNKNOWN;

	bool loaded = false;
};
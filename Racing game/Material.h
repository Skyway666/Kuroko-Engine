#pragma once
#include "Globals.h"


enum Mat_Wrap { W_UNKNOWN, CLAMP, CLAMP_TO_BORDER, REPEAT, MIRRORED_REPEAT };
enum Mat_MinMagFilter { M_UNKNOWN, NEAREST, LINEAR, MIPMAP_NEAREST, MIPMAP_LINEAR};

#define SIZE_CHECKERED 64

class Material
{
public:

	Material() {};
	~Material() {};
	bool setParameters(Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter);
	void LoadCheckered();
	bool isLoaded() { return loaded; };
	uint getGLid() { return id; };

private:
	void LoadToVRAM(const void* pixels, Mat_Wrap wrap = REPEAT, Mat_MinMagFilter min_filter = LINEAR, Mat_MinMagFilter mag_filter = LINEAR);


private:
	uint id = 0;
	uint size_x = 0;
	uint size_y = 0;

	Mat_Wrap wrap_mode = W_UNKNOWN;
	Mat_MinMagFilter min_filter_mode = M_UNKNOWN;
	Mat_MinMagFilter mag_filter_mode = M_UNKNOWN;


	bool loaded = false;
};
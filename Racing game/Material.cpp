#include "Material.h"

#include "glew-2.1.0\include\GL\glew.h"


void Material::LoadToVRAM(const void* pixels, Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	setParameters(wrap, min_filter, mag_filter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE_CHECKERED, SIZE_CHECKERED, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}


bool Material::setParameters(Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter)
{
	wrap_mode = wrap;
	switch (wrap)
	{
	case CLAMP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	case REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	default: 
		return false;
	}

	mag_filter_mode = mag_filter;
	switch (mag_filter)
	{
	case LINEAR:	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); break;
	case NEAREST:	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); break;
	default:
		return false;
	}

	min_filter_mode = min_filter;
	switch (min_filter)
	{
	case LINEAR:			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
	case NEAREST:			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
	case MIPMAP_NEAREST:	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
	case MIPMAP_LINEAR:		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
	default:
		return false;
	}

	return true;
}

void Material::LoadCheckered()
{
	size_x = size_y = SIZE_CHECKERED;
	GLubyte pixels[SIZE_CHECKERED][SIZE_CHECKERED][4];
	for (int i = 0; i < SIZE_CHECKERED; i++) {
		for (int j = 0; j < SIZE_CHECKERED; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			pixels[i][j][0] = (GLubyte)c;
			pixels[i][j][1] = (GLubyte)c;
			pixels[i][j][2] = (GLubyte)c;
			pixels[i][j][3] = (GLubyte)255;
		}
	}
	LoadToVRAM((void*) pixels);
	loaded = true;
}
#include "Material.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGUI.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "Applog.h"


Material::Material()
{
	id = ++App->scene_intro->last_mat_id;
}
Material::~Material()
{
	if (diffuse) delete diffuse;
	if (ambient) delete ambient;
	if (normals) delete normals;
	if (lightmap) delete lightmap;
}


Texture* Material::getTexture(TextureType tex_type)
{
	switch (tex_type)
	{
	case DIFFUSE: return diffuse; break;
	case AMBIENT: return ambient; break;
	case NORMALS: return normals; break;
	case LIGHTMAP: return lightmap; break;
	default: return nullptr;
	}
}

void Material::setTexture(TextureType tex_type, Texture* texture)
{
	switch (tex_type)
	{
	case DIFFUSE: if (diffuse) delete diffuse; diffuse = texture; break;
	case AMBIENT: if (ambient) delete ambient; ambient = texture; break;
	case NORMALS: if (normals) delete normals; normals = texture; break;
	case LIGHTMAP: if (lightmap) delete lightmap; lightmap = texture; break;
	}
}

void Material::setCheckeredTexture(TextureType tex_type)
{
	Texture* texture = new Texture();
	texture->LoadCheckered();

	switch (tex_type)
	{
	case DIFFUSE: if (diffuse) delete diffuse; diffuse = texture; break;
	case AMBIENT: if (ambient) delete ambient; ambient = texture; break;
	case NORMALS: if (normals) delete normals; normals = texture; break;
	case LIGHTMAP: if (lightmap) delete lightmap; lightmap = texture; break;
	}
}

Texture::Texture(uint GL_id)
{ 
	gl_id = GL_id;

	glBindTexture(GL_TEXTURE_2D, gl_id);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&size_x);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&size_y);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &gl_id);
}
void Texture::setParameters(Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter)
{
	bool incompatible_parameter = false;
	glBindTexture(GL_TEXTURE_2D, gl_id);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		incompatible_parameter =  true;
	}

	mag_filter_mode = mag_filter;
	switch (mag_filter)
	{
	case LINEAR:	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); break;
	case NEAREST:	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); break;
		incompatible_parameter = true;
	}

	min_filter_mode = min_filter;
	switch (min_filter)
	{
	case LINEAR:			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
	case NEAREST:			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
	case MIPMAP_NEAREST:	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
	case MIPMAP_LINEAR:		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
		incompatible_parameter = true;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	if (incompatible_parameter)
		app_log->AddLog("error setting texture parameters");
}

void Texture::LoadCheckered()
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

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gl_id);
	setParameters(REPEAT, LINEAR, LINEAR);
	glBindTexture(GL_TEXTURE_2D, gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE_CHECKERED, SIZE_CHECKERED, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

}
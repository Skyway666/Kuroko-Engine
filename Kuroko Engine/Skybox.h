#ifndef _SKYBOX_
#define _SKYBOX_

#include <array>
#include <string>

#include "MathGeoLib\Math\float4x4.h"

class Texture;
class Mesh;

enum Direction { LEFT, RIGHT, UP, DOWN, BACK, FRONT};

class Skybox
{
public:
	Skybox(float distance = 500.0f);

	void updatePosition(const float3& new_pos) { transform_mat.SetTranslatePart(new_pos); };
	void setAllTextures(std::array<Texture*, 6>& texs)	{ for(int i = 0; i < 6; i++)	textures[i] = texs[i]; };
	void setTexture(Texture* tex, Direction index)		{ if (index >= 0 && index < 6)	textures[index] = tex; };
	void Draw() const;

private:
	std::array<Texture*, 6> textures;
	std::array<Mesh*, 6> planes;   
	float distance = 0.0f;
	float4x4 transform_mat = float4x4::identity;
};


#endif
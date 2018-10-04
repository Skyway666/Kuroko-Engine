#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "Vector3.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	SDL_GLContext getContext()	{ return context; }

	float4x4 CreatePerspMat(float fov, float aspect_ratio, float near_plane, float far_plane);

	void DirectDrawCube(Vector3f size);

	bool global_wireframe;
	// JUST FOR LEARNING PURPUSES
	void HomeworksInit();
	void HomeworksUpdate();

	bool draw_sphere;

	// Sphere
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<unsigned short> indices;


	//uint sphereVID, sphereIID, sphereNID, sphereTID;

	// JUST FOR LEARNING PURPUSES

private:




	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

};
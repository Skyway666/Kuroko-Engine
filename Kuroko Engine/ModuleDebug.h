#pragma once

#include "Globals.h"
#include "Module.h"
#include "Color.h"

#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\float2.h"
#include "MathGeoLib\Geometry\Frustum.h"
#include "MathGeoLib\Math\Quat.h"

#include <list>

enum ShapeType { S_UNKNOWN, CUBE, SPHERE, ARROW, AXIS, RAY, FRUSTUM};

struct DebugShape
{
	~DebugShape();
	ShapeType type = S_UNKNOWN;

	float2* lines = nullptr;
	float3* vertices = nullptr;
	float3* normals = nullptr;
	float3* colors = nullptr;

	uint id = 0;
	uint vboId = 0;
	uint iboId = 0;

	uint num_lines = 0;
	uint num_vertices = 0;

	void Draw();
	void LoadDataToVRAM();
};

class ModuleDebug : public Module
{
public:
	ModuleDebug(Application* app, bool start_enabled = true) : Module(app, start_enabled) { name = "debug"; };
	~ModuleDebug() {};

	bool Init(JSON_Object* config) { return true; };
	update_status Update(float dt);
	bool CleanUp();

	uint addArrow(float3 start_point, float3 end_point, Color color = Red);
	uint addAxis(float3 position, float length = 1.0f, Quat rotation = Quat::identity);
	uint addRay(float3 start_point, float3 end_point, Color color = Green);
	uint addFrustum(float3 pos, Quat rotation = Quat::identity, FrustumType type = PerspectiveFrustum, float n_plane = 0.5f, float f_plane = 10.0f, float h_fov_or_ortho_width = 90.0f, float v_fov_or_ortho_height = 59.0f, Color color = Blue);

	void removeShape(uint id);
	void ClearShapes() { shapes.clear(); };

private:
	std::list<DebugShape*> shapes;
	uint last_id = 0;
};
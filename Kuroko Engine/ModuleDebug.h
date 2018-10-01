#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include "Globals.h"
#include "Module.h"
#include "Color.h"

#include <list>

enum ShapeType { S_UNKNOWN, CUBE, SPHERE, ARROW, AXIS, RAY, FRUSTUM};

struct DebugShape
{
	~DebugShape();
	ShapeType type = S_UNKNOWN;

	Point3ui* tris = nullptr;
	Point3f* vertices = nullptr;
	Point3f* normals = nullptr;
	Point3f* colors = nullptr;

	uint id = 0;
	uint vboId = 0;
	uint iboId = 0;

	uint num_tris = 0;
	uint num_vertices = 0;

	bool wireframe = false;
	bool face_culling = true;

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

	uint addArrow(Vector3f start_point, Vector3f end_point, Color color = Red);
	uint addAxis(Vector3f position, float length = 1.0f, Quat rotation = Quat::identity);
	uint addRay(Vector3f start_point, Vector3f end_point, Color color = Green);

	void removeShape(uint id);
	void ClearShapes() { shapes.clear(); };

private:
	std::list<DebugShape*> shapes;
	uint last_id = 0;
};
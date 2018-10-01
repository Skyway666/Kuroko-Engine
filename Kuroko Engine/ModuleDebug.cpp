#include "ModuleDebug.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

bool ModuleDebug::CleanUp()
{
	shapes.clear();
	return true;
}

update_status ModuleDebug::Update(float dt)
{
	for (std::list<DebugShape*>::iterator it = shapes.begin(); it != shapes.end(); it++)
		(*it)->Draw();

	return UPDATE_CONTINUE;
}

uint ModuleDebug::addArrow(Vector3f start_point, Vector3f end_point, Color color)
{
	DebugShape* arrow = new DebugShape();
	arrow->type = ARROW;
	Vector3f aux = end_point - start_point;

	arrow->num_vertices = 6;
	arrow->vertices = new Vector3f[6];

	float3 perp1 = aux.toMathVec().Perpendicular().Normalized() * 0.3f;
	float3 perp2 = aux.toMathVec().AnotherPerpendicular().Normalized() * 0.3f;
	arrow->vertices[0] = start_point;			arrow->vertices[1] = end_point;
	arrow->vertices[2] = (aux* 0.95f) - (perp1); arrow->vertices[3] = (aux* 0.95f) + (perp1);
	arrow->vertices[4] = (aux* 0.95f) - (perp2); arrow->vertices[5] = (aux* 0.95f) + (perp2);
	
	arrow->num_tris = 3;
	arrow->tris = new Point3ui[3];
	arrow->tris[0] = { 0,1,0 }; arrow->tris[1] = { 2,1,3 }; arrow->tris[2] = { 4,1,5 };

	arrow->colors = new Vector3f[6];
	for (int i = 0; i < 6; i++)
		arrow->colors[i] = { color.r, color.g, color.b };

	arrow->normals = new Vector3f[6];
	aux.Normalize();
	arrow->normals[0] = aux.Negate(); arrow->normals[1] = aux;
	arrow->normals[2] = (arrow->vertices[2] - end_point).Normalized(); arrow->normals[3] = (arrow->vertices[3] - end_point).Normalized();
	arrow->normals[4] = (arrow->vertices[4] - end_point).Normalized(); arrow->normals[5] = (arrow->vertices[5] - end_point).Normalized();

	arrow->id = last_id++;
	arrow->wireframe = true;
	arrow->enable_face_culling = false;
	arrow->LoadDataToVRAM();

	shapes.push_back(arrow);
	return arrow->id;
}

uint ModuleDebug::addAxis(Vector3f position, float length, Quat rotation)
{
	Vector3f X = { length, 0.0f, 0.0f };
	Vector3f Y = { 0.0f, length, 0.0f };
	Vector3f Z = { 0.0f, 0.0f, length };

	if (!rotation.Equals(Quat::identity))
	{
		X = rotation * X.toMathVec();
		Y = rotation * Y.toMathVec();
		Z = rotation * Z.toMathVec();
	}

	DebugShape* axis = new DebugShape();
	axis->type = AXIS;

	axis->num_vertices = 4;
	axis->vertices = new Vector3f[4];
	axis->vertices[0] = position; axis->vertices[1] = position + X; axis->vertices[2] = position + Y; axis->vertices[3] = position + Z;

	axis->num_tris = 3;
	axis->tris = new Point3ui[3];
	axis->tris[0] = { 1,0,1 }; axis->tris[1] = { 2,0,2 }; axis->tris[2] = { 3,0,3 };

	axis->colors = new Vector3f[4];
	axis->colors[0] = { 1.0f, 1.0f, 1.0f }; axis->colors[1] = { 1.0f, 0.0f, 0.0f }; 
	axis->colors[2] = { 0.0f, 1.0f, 0.0f }; axis->colors[3] = { 0.0f, 0.0f, 1.0f };

	axis->normals = new Vector3f[4];
	axis->normals[0] = Vector3f::One;			 axis->normals[1] = X.Negate().Normalized();
	axis->normals[2] = Y.Negate().Normalized();  axis->normals[3] = Z.Negate().Normalized();

	axis->id = last_id++;
	axis->wireframe = true;
	axis->enable_face_culling = false;
	axis->LoadDataToVRAM();

	shapes.push_back(axis);
	return axis->id;
}

void ModuleDebug::removeShape(uint id)
{
	DebugShape* to_erase = nullptr;

	for (std::list<DebugShape*>::iterator it = shapes.begin(); it != shapes.end(); it++)
	{
		if ((*it)->id == id)
		{
			to_erase = *it;
			break;
		}
	}

	shapes.remove(to_erase);
}



void DebugShape::Draw()
{
	// bind VBOs before drawing
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

	// enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	if (wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glLineWidth(3.0f); }
	else			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	bool previous_setting = glIsEnabled(GL_CULL_FACE);
	if (enable_face_culling != previous_setting)
		enable_face_culling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	size_t Offset = sizeof(Vector3f) * num_vertices;

	// specify vertex arrays with their offsets
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	glNormalPointer(GL_FLOAT, 0, (void*)Offset);
	glColorPointer(3, GL_FLOAT, 0, (void*)(Offset * 2));

	glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, NULL);

	if (enable_face_culling != previous_setting)
		previous_setting ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void DebugShape::LoadDataToVRAM()
{
	// create VBOs
	glGenBuffers(1, &vboId);    // for vertex buffer
	glGenBuffers(1, &iboId);    // for index buffer

	size_t vSize = sizeof(Vector3f) * num_vertices;
	size_t tSize = sizeof(Point2f) * num_vertices;

	// copy vertex attribs data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vSize + vSize + vSize + tSize, 0, GL_STATIC_DRAW); // reserve space
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);                  // copy verts at offset 0
	glBufferSubData(GL_ARRAY_BUFFER, vSize, vSize, normals);               // copy norms after verts
	glBufferSubData(GL_ARRAY_BUFFER, vSize + vSize, vSize, colors);          // copy cols after norms
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// copy index data to VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3ui) * num_tris, tris, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

DebugShape::~DebugShape()
{
	glDeleteBuffers(1, &iboId);
	glDeleteBuffers(1, &vboId);

	if (vertices)	delete vertices;
	if (tris)		delete tris;
	if (normals)		delete normals;
	if (colors)		delete colors;
}
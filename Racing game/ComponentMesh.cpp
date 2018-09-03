#include "ComponentMesh.h"
#include "Globals.h"

#include "glew-2.1.0\include\GL\glew.h"

#include "Assimp\include\scene.h"



ComponentMesh::ComponentMesh(GameObject* gameobject, PrimitiveTypes primitive) : Component(gameobject, MESH)
{
	switch (primitive)
	{
	case Primitive_Cube:	BuildCube();
		break;

	default:
		break;
	}

	LoadDataToVRAM();
	loaded = true;
}

ComponentMesh::ComponentMesh(GameObject* gameobject, aiMesh* imported_mesh) : Component(gameobject, MESH)
{
	if (LoadFromAssimpMesh(imported_mesh))
	{
		LoadDataToVRAM();
		loaded = true;
	}
	else
		APPLOG("error loading mesh for the component %s", imported_mesh->mName.C_Str());
}


ComponentMesh::~ComponentMesh()
{
	if (vertices) delete vertices;
	if (tris) delete tris;
	if (normals) delete normals;
}

void ComponentMesh::LoadDataToVRAM()
{
	if (num_tris > 0)
	{
		glGenBuffers(1, (GLuint*) &(id_tris));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tris);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Point3ui) * num_tris, tris, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}

	if (num_vertices > 0) 
	{
		glGenBuffers(1, (GLuint*) &(id_vertices));
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point3f) * num_vertices, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	if (num_normals > 0) {

		glGenBuffers(1, (GLuint*) &(id_normals));
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point3f) * num_normals, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}



void ComponentMesh::Draw() {

	// early exit if empty mesh
	if (num_tris == 0 || num_vertices == 0)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tris);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_NORMAL_ARRAY, id_normals);

	if (wireframe)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 1.0f, 1.0f);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}


void ComponentMesh::BuildCube(float sx, float sy, float sz)
{
	sx *= 0.5f, sy *= 0.5f, sz *= 0.5f;

	num_vertices = 8;
	vertices = new Point3f[num_vertices];

	vertices[0].set(-sx, -sy, -sz);
	vertices[1].set(sx, -sy, -sz);
	vertices[2].set(-sx, -sy, sz);
	vertices[3].set(sx, -sy, sz);

	vertices[4].set(-sx, sy, -sz);
	vertices[5].set(sx, sy, -sz);
	vertices[6].set(-sx, sy, sz);
	vertices[7].set(sx, sy, sz);

	num_tris = 12;
	tris = new Point3ui[num_tris];

	tris[0].set(0, 1, 2);	tris[1].set(3, 2, 1);	//front
	tris[2].set(6, 5, 4);	tris[3].set(5, 6, 7);	//back
	tris[4].set(5, 3, 1);	tris[5].set(3, 5, 7);	//up
	tris[6].set(0, 2, 4);	tris[7].set(6, 4, 2);	//down
	tris[8].set(4, 1, 0);	tris[9].set(1, 4, 5);	//left
	tris[10].set(2, 3, 6);	tris[11].set(7, 6, 3);	//right  
}


bool ComponentMesh::LoadFromAssimpMesh(aiMesh* imported_mesh)
{
	if (imported_mesh->mNumVertices)
	{
		num_vertices = imported_mesh->mNumVertices;
		vertices = new Point3f[num_vertices];
		memcpy(vertices, imported_mesh->mVertices, sizeof(Point3f) * num_vertices);


		if (imported_mesh->HasFaces())
		{
			num_tris = imported_mesh->mNumFaces;
			tris = new Point3ui[num_tris]; // assume each face is a triangle
			for (uint i = 0; i < num_tris; ++i)
			{
				if (imported_mesh->mFaces[i].mNumIndices == 3)
					memcpy(&tris[i], imported_mesh->mFaces[i].mIndices, sizeof(Point3ui));
				else
					APPLOG("WARNING, geometry face with != 3 indices!");
			}

			return true;
		}
	}
	return false;
}

void ComponentMesh::getData(uint& vert_num, uint& poly_count, bool& has_normals)
{
	vert_num = num_vertices;
	poly_count = num_tris;
	has_normals = (num_normals > 0);
}

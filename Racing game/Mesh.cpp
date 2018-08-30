#include "Mesh.h"

#include "glew-2.1.0\include\GL\glew.h"

void Mesh::LoadDataToVRAM()
{
	if (num_tris > 0)
	{
		glGenBuffers(1, (GLuint*) &(id_tris));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tris);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Point3i) * num_tris, tris, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}

	if (num_vertices > 0) {

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



void Mesh::Draw() {

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tris);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_NORMAL_ARRAY, id_normals);

	glColor3f(1.0f, 1.0f, 1.0f);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}

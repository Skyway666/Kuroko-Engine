#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleImGUI.h"
#include "Globals.h"
#include "Applog.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#pragma comment( lib, "glew-2.1.0/lib/glew32.lib")
#pragma comment( lib, "glew-2.1.0/lib/glew32s.lib")


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "renderer3d";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(JSON_Object* config)
{
	app_log->AddLog("Creating 3D Renderer context...\n");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->main_window->window);
	if(context == NULL)
	{
		app_log->AddLog("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		GLenum err = glewInit();

		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			app_log->AddLog("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClearColor(0.8f, 0.8f, 0.8f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

	}
	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	HomeworksInit();
	draw_direct_cube = false;
	draw_buffer_cube = false;
	draw_index_cube = false;
	draw_sphere = false;
	draw_cylinder = false;

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());
	
	HomeworksUpdate();
	// light 0 on cam pos


	//lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	//for(uint i = 0; i < MAX_LIGHTS; ++i)
	//	lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(App->window->main_window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	app_log->AddLog("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ProjectionMatrix = CreatePerspMat(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf((GLfloat*)ProjectionMatrix.v);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float4x4 ModuleRenderer3D::CreatePerspMat(float fov, float aspect_ratio, float near_plane, float far_plane)
{
	float4x4 Perspective = float4x4::zero;
	
	float coty = 1.0f / tan(fov * (float)M_PI / 360.0f);

	Perspective.v[0][0] = coty / aspect_ratio;
	Perspective.v[1][1] = coty;
	Perspective.v[2][2] = (near_plane + far_plane) / (near_plane - far_plane);
	Perspective.v[2][3] = -1.0f;
	Perspective.v[3][2] = 2.0f * near_plane * far_plane / (near_plane - far_plane);
	Perspective.v[3][3] = 0.0f;

	return Perspective;
}

void ModuleRenderer3D::DirectDrawCube(Vector3f size)
{
	glLineWidth(2.0f);

	size.x *= 0.5f; size.y *= 0.5f; size.z *= 0.5f;

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-size.x, -size.y, size.z);
	glVertex3f(size.x, -size.y, size.z);
	glVertex3f(size.x, size.y, size.z);
	glVertex3f(-size.x, size.y, size.z);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(size.x, -size.y, -size.z);
	glVertex3f(-size.x, -size.y, -size.z);
	glVertex3f(-size.x, size.y, -size.z);
	glVertex3f(size.x, size.y, -size.z);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(size.x, -size.y, size.z);
	glVertex3f(size.x, -size.y, -size.z);
	glVertex3f(size.x, size.y, -size.z);
	glVertex3f(size.x, size.y, size.z);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-size.x, -size.y, -size.z);
	glVertex3f(-size.x, -size.y, size.z);
	glVertex3f(-size.x, size.y, size.z);
	glVertex3f(-size.x, size.y, -size.z);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-size.x, size.y, size.z);
	glVertex3f(size.x, size.y, size.z);
	glVertex3f(size.x, size.y, -size.z);
	glVertex3f(-size.x, size.y, -size.z);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-size.x, -size.y, -size.z);
	glVertex3f(size.x, -size.y, -size.z);
	glVertex3f(size.x, -size.y, size.z);
	glVertex3f(-size.x, -size.y, size.z);

	glEnd();

}

void ModuleRenderer3D::HomeworksInit() {

	// Cube no index
	my_cubeid = 0;
	float float_array[108] = {
		//face 1
		0.f, 0.f, 0.f, //0
		0.f, 0.f, 10.f, // 1
		0.f, 10.f, 0.f, // 2

		0.f, 10.f, 10.f,//4
		0.f, 10.f, 0.f,//2
		0.f, 0.f, 10.f, //1
		//face 2
		10.f, 10.f, 0.f,//5
		0.f, 10.f, 0.f,//2
		0.f, 10.f, 10.f,//4

		0.f, 10.f, 10.f,//4
		10.f, 10.f, 10.f,//7
		10.f, 10.f, 0.f,//5
		//face 3
		10.f, 0.f, 10.f,//6
		10.f, 0.f, 0.f, //3
		10.f, 10.f, 0.f,//5

		10.f, 10.f, 0.f,//5
		10.f, 10.f, 10.f,//7
		10.f, 0.f, 10.f,//6
		//face 4
		10.0f, 0.f, 0.f,//3
		10.f, 0.f, 10.f,//6
		0.f, 0.f, 10.f, //1

		0.f, 0.f, 10.f, //1
		0.f, 0.f, 0.f, //0
		10.f, 0.f, 0.f,//3
		//face 5
		10.f, 0.f, 10.f,//6
		10.f, 10.f, 10.f,//7
		0.f, 10.f, 10.f,//4

		0.f, 10.f, 10.f,//4
		0.f, 0.f, 10.f,//1
		10.f, 0.f, 10.f,//6
		//face 6
		10.f, 0.f, 0.f,//3
		0.f, 0.f, 0.f,//0
		0.f, 10.f, 0.f,//2

		0.f, 10.f, 0.f,//2
		10.f, 10.f, 0.f,//5
		10.f, 0.f, 0.f//3

	};


	glGenBuffers(1, (GLuint*)&my_cubeid);
	glBindBuffer(GL_ARRAY_BUFFER, my_cubeid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, (void*)float_array, GL_STATIC_DRAW);

	// Cube with index

	cube_vertices[0] = 0.f; cube_vertices[1] = 0.f; cube_vertices[2] = 0.f; //0
	cube_vertices[3] = 0.f; cube_vertices[4] = 0.f; cube_vertices[5] = 10.f; //1
	cube_vertices[6] = 0.f; cube_vertices[7] = 10.f; cube_vertices[8] = 0.f; //2
	cube_vertices[9] = 10.f; cube_vertices[10] = 0.f; cube_vertices[11] = 0.f;//3

	cube_vertices[12] = 0.f; cube_vertices[13] = 10.f; cube_vertices[14] = 10.f;//4
	cube_vertices[15] = 10.f; cube_vertices[16] = 10.f; cube_vertices[17] = 0.f;//5
	cube_vertices[18] = 10.f; cube_vertices[19] = 0.f; cube_vertices[20] = 10.f;//6
	cube_vertices[21] = 10.f; cube_vertices[22] = 10.f; cube_vertices[23] = 10.f;//7

	cube_indices[0] = 0; cube_indices[1] = 1; cube_indices[2] = 2;   	cube_indices[3] = 4; cube_indices[4] = 2; cube_indices[5] = 1; // face 1
	cube_indices[6] = 5; cube_indices[7] = 2; cube_indices[8] = 4;   	cube_indices[9] = 4; cube_indices[10] = 7; cube_indices[11] = 5; // face 2
	cube_indices[12] = 6; cube_indices[13] = 3; cube_indices[14] = 5;   	cube_indices[15] = 5; cube_indices[16] = 7; cube_indices[17] = 6; // face 3
	cube_indices[18] = 3; cube_indices[19] = 6; cube_indices[20] = 1;   	cube_indices[21] = 1; cube_indices[22] = 0; cube_indices[23] = 3; // face 4
	cube_indices[24] = 6; cube_indices[25] = 7; cube_indices[26] = 4;   	cube_indices[27] = 4; cube_indices[28] = 1; cube_indices[29] = 6; // face 5
	cube_indices[30] = 3; cube_indices[31] = 0; cube_indices[32] = 2;   	cube_indices[33] = 2; cube_indices[34] = 5; cube_indices[35] = 3; // face 6

	

	// Sphere (codde from http://www.songho.ca/opengl/gl_sphere.html)

	float radius = 1.0f;
	float sectorCount = 12.0f;
	float stackCount = 24.0f;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

													// add (sectorCount+1) vertices per stack
													// the first and last vertices have same position and normal, but different tex coods
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);

			// vertex tex coord (s, t)
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			texcoords.push_back(s);
			texcoords.push_back(t);
		}
	}
	int k1, k2;
	for (int i = 0; i < stackCount; ++i) {
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}


	//glGenBuffers(1, &sphereVID);
	//glBindBuffer(GL_ARRAY_BUFFER, sphereVID);
	//glBufferData(GL_ARRAY_BUFFER,						// target
	//	vertices.size()*sizeof(float),					// data size, bytes
	//	&vertices[0],									// ptr to vertex data
	//	GL_STATIC_DRAW);								// usage

	//									   // copy index data to VBO

	//glGenBuffers(1, &sphereIID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER,               // target
	//	indices.size()*sizeof(short),                   // data size, bytes
	//	&indices[0],									// ptr to index data
	//	GL_STATIC_DRAW);						        // usage

	//glGenBuffers(1, &sphereNID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereNID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER,               // target
	//	normals.size() * sizeof(short),                 // data size, bytes
	//	&normals[0],									// ptr to index data
	//	GL_STATIC_DRAW);						        // usage

	//glGenBuffers(1, &sphereTID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereTID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER,               // target
	//	texcoords.size() * sizeof(short),               // data size, bytes
	//	&texcoords[0],									// ptr to index data
	//	GL_STATIC_DRAW);						        // usage

}



void ModuleRenderer3D::HomeworksUpdate() {
	if(draw_direct_cube){
	//glBegin(GL_TRIANGLES);
	//// face 1
	//glVertex3f(0.f, 0.f, 0.f);
	//glVertex3f(0.f, 0.f, 10.f);
	//glVertex3f(0.f, 10.f, 0.f);

	//glVertex3f(0.f, 10.f, 10.f);
	//glVertex3f(0.f, 10.f, 0.f);
	//glVertex3f(0.f, 0.f, 10.f);
	//// face 2
	//glVertex3f(10.f, 10.f, 0.f); 
	//glVertex3f(0.f, 10.f, 0.f);
	//glVertex3f(0.f, 10.f, 10.f);

	//glVertex3f(0.f, 10.f, 10.f);
	//glVertex3f(10.f, 10.f, 10.f);
	//glVertex3f(10.f, 10.f, 0.f);
	//// face 3
	//glVertex3f(10.f, 0.f, 10.f);
	//glVertex3f(10.f, 0.f, 0.f);
	//glVertex3f(10.f, 10.f, 0.f);

	//glVertex3f(10.f, 10.f, 0.f);
	//glVertex3f(10.f, 10.f, 10.f);
	//glVertex3f(10.f, 0.f, 10.f);
	//// face 4
	//glVertex3f(10.0f, 0.f, 0.f);
	//glVertex3f(10.f, 0.f, 10.f);
	//glVertex3f(0.f, 0.f, 10.f);

	//glVertex3f(0.f, 0.f, 10.f);
	//glVertex3f(0.f, 0.f, 0.f);
	//glVertex3f(10.f, 0.f, 0.f);
	//// face 5
	//glVertex3f(10.f, 0.f, 10.f);
	//glVertex3f(10.f, 10.f, 10.f);
	//glVertex3f(0.f, 10.f, 10.f);

	//glVertex3f(0.f, 10.f, 10.f);
	//glVertex3f(0.f, 0.f, 10.f);
	//glVertex3f(10.f, 0.f, 10.f);
	//// face 6
	//glVertex3f(10.f, 0.f, 0.f);
	//glVertex3f(0.f, 0.f, 0.f);
	//glVertex3f(0.f, 10.f, 0.f);

	//glVertex3f(0.f, 10.f, 0.f);
	//glVertex3f(10.f, 10.f, 0.f);
	//glVertex3f(10.f, 0.f, 0.f);
	//glEnd();
	//glLineWidth(1.0f);
	}
	if(draw_buffer_cube){
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, my_cubeid);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if (draw_index_cube) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
		// draw a cube
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, cube_indices);

		// deactivate vertex arrays after drawing
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	else if(draw_sphere){




		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);









		//glBindBuffer(GL_ARRAY_BUFFER, sphereVID);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIID);

		//glEnableVertexAttribArray(sphereVID);
		//glEnableVertexAttribArray(sphereNID);
		//glEnableVertexAttribArray(sphereTID);



		//glVertexAttribPointer(sphereVID, 3, GL_FLOAT, false, stride, 0);
		//glVertexAttribPointer(sphereNID, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
		//glVertexAttribPointer(sphereTID, 2, GL_FLOAT, false, stride, (void*)(sizeof(float) * 6));



		//glDrawElements(GL_TRIANGLES,                  // primitive type
		//	indices.size(),								// # of indices
		//	GL_UNSIGNED_SHORT,							// data type
		//	(void*)0);								    // offset to indices

		//glDisableVertexAttribArray(sphereVID);
		//glDisableVertexAttribArray(sphereNID);
		//glDisableVertexAttribArray(sphereTID);

		//// unbind
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	}
}

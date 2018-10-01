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
	draw_sphere = true;
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
		0.f, 0.f, 0.f,
		0.f, 0.f, 10.f,
		0.f, 10.f, 0.f,

		0.f, 10.f, 10.f,
		0.f, 10.f, 0.f,
		0.f, 0.f, 10.f,
		//face 2
		10.f, 10.f, 0.f,
		0.f, 10.f, 0.f,
		0.f, 10.f, 10.f,

		0.f, 10.f, 10.f,
		10.f, 10.f, 10.f,
		10.f, 10.f, 0.f,
		//face 3
		10.f, 0.f, 10.f,
		10.f, 0.f, 0.f,
		10.f, 10.f, 0.f,

		10.f, 10.f, 0.f,
		10.f, 10.f, 10.f,
		10.f, 0.f, 10.f,
		//face 4
		10.0f, 0.f, 0.f,
		10.f, 0.f, 10.f,
		0.f, 0.f, 10.f,

		0.f, 0.f, 10.f,
		0.f, 0.f, 0.f,
		10.f, 0.f, 0.f,
		//face 5
		10.f, 0.f, 10.f,
		10.f, 10.f, 10.f,
		0.f, 10.f, 10.f,

		0.f, 10.f, 10.f,
		0.f, 0.f, 10.f,
		10.f, 0.f, 10.f,
		//face 6
		10.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 10.f, 0.f,

		0.f, 10.f, 0.f,
		10.f, 10.f, 0.f,
		10.f, 0.f, 0.f

	};


	glGenBuffers(1, (GLuint*)&my_cubeid);
	glBindBuffer(GL_ARRAY_BUFFER, my_cubeid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, (void*)float_array, GL_STATIC_DRAW);

	// Cube with index
	float radius = 10.0f;
	int rings = 100;
	int sectors = 200;

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	texcoords.resize(rings * sectors * 2);
	std::vector<float>::iterator v = vertices.begin();
	std::vector<float>::iterator n = normals.begin();
	std::vector<float>::iterator t = texcoords.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		*t++ = s * S;
		*t++ = r * R;

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

		*n++ = x;
		*n++ = y;
		*n++ = z;
	}

	indices.resize(rings * sectors * 4);
	std::vector<unsigned short>::iterator i = indices.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}
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

	}
	else if(draw_sphere){
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
		glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
		glPopMatrix();
	}
}

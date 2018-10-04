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
	draw_sphere = false;
	global_wireframe = false;
	global_normals = false;

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

	
	// Sphere (codde from http://www.songho.ca/opengl/gl_sphere.html)
	{
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

}



void ModuleRenderer3D::HomeworksUpdate() {
	
	if(draw_sphere){
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

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "Globals.h"
#include "Applog.h"
#include "ModuleImporter.h"

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
{
	for (auto it = frame_buffers.begin(); it != frame_buffers.end(); it++)
		delete *it;

	frame_buffers.clear();
	frame_buffers_to_delete.clear();
}

// Called before render is available
bool ModuleRenderer3D::Init(const JSON_Object& config)
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
		//glEnable(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

	}
	// Projection matrix for
	OnResize(App->window->main_window->width, App->window->main_window->height);
	initFrameBuffer();
	return ret;
}

bool ModuleRenderer3D::Start()
{
	App->importer->Import("BakerHouse.fbx");
	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->editor_camera->GetViewMatrix());
	
	lights[0].SetPos(App->camera->editor_camera->Position.x, App->camera->editor_camera->Position.y, App->camera->editor_camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

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

	App->camera->editor_camera->setProjMatrix(App->camera->CreatePerspMat(60.0, width, height, 0.125f, 1250.0f));
	glLoadMatrixf((GLfloat*)App->camera->editor_camera->GetProjMatrix().v);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


FrameBuffer* ModuleRenderer3D::initFrameBuffer(uint size_x, uint size_y)
{
	FrameBuffer* fb = new FrameBuffer();
	fb->size_x = size_x; fb->size_y = size_y;
	// set frame buffer
	glGenFramebuffers(1, &fb->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->id);

	// set frame buffer texture
	fb->tex = new Texture();
	glGenTextures(1, &fb->tex->gl_id);
	glBindTexture(GL_TEXTURE_2D, fb->tex->gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//set depth renderer
	glGenRenderbuffers(1, &fb->depthbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, fb->depthbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size_x, size_y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->depthbuffer_id);

	// init frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex->gl_id, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	frame_buffers.push_back(fb);
	return fb;
}


void ModuleRenderer3D::DirectDrawCube(float3& size) const
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


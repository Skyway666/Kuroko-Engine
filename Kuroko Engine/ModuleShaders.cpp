#include "Application.h"
#include "ModuleShaders.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "Globals.h"
#include "Applog.h"
#include "FileSystem.h"


ModuleShaders::ModuleShaders(Application* app, bool start_enabled):Module(app,start_enabled)
{
	name = "Shaders";
}

ModuleShaders::~ModuleShaders()
{
}

bool ModuleShaders::Init(const JSON_Object * config)
{
	bool ret = true;

	//renderer things they should go back to where they belong
	/*App->renderer3D->context = SDL_GL_CreateContext(App->window->main_window->window);
	if (App->renderer3D->context == NULL)
	{
		app_log->AddLog("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		LOG("Glew library not initialized properly %s\n", glewGetErrorString(err));
		ret = false;
	}
	
	*/
	
	bool findFile = App->fs.ExistisFile(defaultVertexFile.c_str(), LIBRARY_MATERIALS, ".vex");
	if (findFile)
	{
		defVertexShader = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefShaderProgram();
	}

	findFile = App->fs.ExistisFile(defaultFragmentFile.c_str(), LIBRARY_MATERIALS, ".frag");
	if (findFile)
	{
		defVertexShader = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefShaderProgram();
	}

	return ret;
}

void ModuleShaders::CreateDefVertexShader()
{
}

void ModuleShaders::CreateDefFragmentShader()
{
}

void ModuleShaders::CreateDefShaderProgram()
{
}



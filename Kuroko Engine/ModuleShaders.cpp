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

	return ret;
}

bool ModuleShaders::InitializeDefaulShaders()
{
	bool findFile = App->fs.ExistisFile(defaultVertexFile.c_str(), LIBRARY_MATERIALS, ".vex");
	if (findFile)
	{
		defVertexShader = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefVertexShader();
	}

	findFile = App->fs.ExistisFile(defaultFragmentFile.c_str(), LIBRARY_MATERIALS, ".frag");
	if (findFile)
	{
		defVertexShader = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefFragmentShader();
	}

	return true;
}

void ModuleShaders::CreateDefVertexShader()
{
	defVertexShader =
	"#version 330\n"

	"layout(location = 0)in vec4 vert;\n"

	"uniform mat4 projection;\n"
	"uniform mat4 view;\n"
	"uniform mat4 model;\n"

	"void main()\n"
	"{\n"
		"gl_Position = projection * view * model * vert;\n"
	"}\n";
}

void ModuleShaders::CreateDefFragmentShader()
{
	defFragmentShader =
	"#version 330\n"

	"out vec4 fragColor;\n"

	"void main()\n"
	"{\n"
		"fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n";
}

void ModuleShaders::CreateDefShaderProgram()
{
}



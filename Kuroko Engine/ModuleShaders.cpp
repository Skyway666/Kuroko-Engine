#include "Application.h"
#include "ModuleShaders.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "Globals.h"
#include "Applog.h"
#include "FileSystem.h"

#include "glew-2.1.0/include/GL/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment( lib, "glew-2.1.0/lib/glew32.lib")
#pragma comment( lib, "glew-2.1.0/lib/glew32s.lib")


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

	defVertexShader = new Shader(VERTEX);
	defFragmentShader = new Shader(FRAGMENT);

	return ret;
}

bool ModuleShaders::InitializeDefaulShaders()
{
	bool findFile = App->fs.ExistisFile(defaultVertexFile.c_str(), LIBRARY_MATERIALS, ".vex");
	if (findFile)
	{
		defVertexShader->script = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefVertexShader();
		CompileShader(defVertexShader);
	}

	findFile = App->fs.ExistisFile(defaultFragmentFile.c_str(), LIBRARY_MATERIALS, ".frag");
	if (findFile)
	{
		defVertexShader->script = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefFragmentShader();
		CompileShader(defFragmentShader);
	}

	return true;
}

void ModuleShaders::CreateDefVertexShader()
{
	defVertexShader->script =
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
	defFragmentShader->script =
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

void ModuleShaders::CompileShader(Shader * shader)
{
	if (shader->type == VERTEX)
	{
		shader->shaderId = glCreateShader(GL_VERTEX_SHADER);
	}
	else
	{
		shader->shaderId = glCreateShader(GL_FRAGMENT_SHADER);
	}
	
	glShaderSource(shader->shaderId, 1, &shader->script, NULL);
	glCompileShader(shader->shaderId);

	int success;
	char Error[512];
	glGetShaderiv(shader->shaderId, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader->shaderId, 512, NULL, Error);
		app_log->AddLog(Error);
	}
	else
	{
		if(shader->type == VERTEX)
			app_log->AddLog("Vertex Shader compiled successfully!");
		else
			app_log->AddLog("Fragment Shader compiled successfully!");
	}
}



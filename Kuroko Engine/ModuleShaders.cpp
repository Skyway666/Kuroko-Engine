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
	RELEASE(defVertexShader);
	RELEASE(defFragmentShader);
	RELEASE(defShaderProgram);
}

bool ModuleShaders::Init(const JSON_Object * config)
{
	bool ret = true;

	defVertexShader = new Shader(VERTEX);
	defFragmentShader = new Shader(FRAGMENT);

	defShaderProgram = new ShaderProgram();

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
	}

	CompileShader(defVertexShader);
	defShaderProgram->shaders.push_back(defVertexShader->shaderId);

	findFile = App->fs.ExistisFile(defaultFragmentFile.c_str(), LIBRARY_MATERIALS, ".frag");
	if (findFile)
	{
		defVertexShader->script = App->fs.ImportFile(defaultVertexFile.c_str());
	}
	else
	{
		CreateDefFragmentShader();
	}

	CompileShader(defFragmentShader);
	defShaderProgram->shaders.push_back(defFragmentShader->shaderId);

	CompileProgram(defShaderProgram);

	return true;
}

void ModuleShaders::CreateDefVertexShader()
{
	defVertexShader->script =
	"#version 330\n"

	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"layout(location = 2) in vec2 texCoord;\n"
	"layout(location = 3) in vec3 normal;\n"

	"out vec4 ourColor;\n"
	"out vec2 TexCoord;\n"
	"out vec3 end_pos;\n"
	"out vec3 ret_normal;\n"

	"uniform mat4 projection;\n"
	"uniform mat4 view;\n"
	"uniform mat4 model;\n"

	"void main()\n"
	"{\n"
		"gl_Position = projection * view * model * vec4(position,1.0f);\n"
		"ourColor = color;\n"
		"TexCoord = texCoord;\n"
		"ret_normal = normal;\n"
	"}\n";
}

void ModuleShaders::CreateDefFragmentShader()
{
	defFragmentShader->script =
	"#version 330\n"
	
	"in vec4 ourColor;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ret_normal;\n"

	"out vec4 color;\n"
	"out vec4 texture_color;\n"
	
	"uniform sampler2D ourTexture;\n"

	"void main()\n"
	"{\n"
		"texture_color = texture(ourTexture, TexCoord);\n"
		"color = ourColor;\n"
	"}\n";
}

void ModuleShaders::CompileShader(Shader* shader)
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

void ModuleShaders::CompileProgram(ShaderProgram* program)
{
	program->programID = glCreateProgram();

	for (int i = 0; i < program->shaders.size(); i++)
	{
		glAttachShader(program->programID, program->shaders[i]);
	}

	glLinkProgram(program->programID);

	int success;
	char Error[512];
	glGetProgramiv(program->programID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program->programID, 512, NULL, Error);
		app_log->AddLog(Error);
	}
	else
	{
		app_log->AddLog("Program linked and compiled Successfully!");
	}

}



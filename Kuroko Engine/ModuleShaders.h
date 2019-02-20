#ifndef __MODULE_SHADERS__
#define __MODULE_SHADERS__

#include "Module.h"

enum ShaderType
{
	VERTEX,
	FRAGMENT
};

class ModuleShaders : public Module
{
public:
	
	ModuleShaders(Application* app, bool start_enabled = true);
	~ModuleShaders();


	bool Init(const JSON_Object* config);

	void CreateDefVertexShader();
	void CreateDefFragmentShader();
	void CreateDefShaderProgram();


private:

	std::string defaultVertexFile = "Assets\\Shaders\\DefaultVertexShader.vex";
	std::string defaultPixelFile = "Assets\\Shaders\\DefaultPixelShader.fmt";

	char* defVertexShader = nullptr;
	char* defFragmentShader = nullptr;

	
	
};

#endif // !__MODULE_SHADERS__

#ifndef __MODULE_SHADERS__
#define __MODULE_SHADERS__

#include "Module.h"

enum ShaderType
{
	VERTEX,
	FRAGMENT
};

struct Shader {

	Shader(ShaderType type) :type(type) {};

	char* script = nullptr;
	ShaderType type;
	uint shaderId=0;

};

class ModuleShaders : public Module
{
public:
	
	ModuleShaders(Application* app, bool start_enabled = true);
	~ModuleShaders();


	bool Init(const JSON_Object* config);

	bool InitializeDefaulShaders();

	void CreateDefVertexShader();
	void CreateDefFragmentShader();
	void CreateDefShaderProgram();

	void CompileShader(Shader* shader);


private:

	std::string defaultVertexFile = "DefaultVertexShader.vex";
	std::string defaultFragmentFile = "DefaultPixelShader.fmt";


	Shader* defVertexShader;
	Shader* defFragmentShader;

	
	
};

#endif // !__MODULE_SHADERS__

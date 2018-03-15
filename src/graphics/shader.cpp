#include <graphics/Shader.h>

#include <fstream>

namespace engine {
}

void checkCompileErrors(unsigned shader, std::string type, const char* shaderName)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			LOGI("ERROR::SHADER_COMPILATION_ERROR of type: %s \n %s \n", type.c_str(), infoLog);
            LOGI("In shader %s\n", shaderName);
			ASSERT(false);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			LOGI("ERROR::PROGRAM_LINKING_ERROR of type: %s \n %s \n", type.c_str(), infoLog);
            LOGI("In shader %s\n", shaderName);
			ASSERT(false);
		}
	}
}

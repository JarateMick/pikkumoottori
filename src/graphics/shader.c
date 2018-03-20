#include <graphics/Shader.h>

void checkCompileErrors(unsigned shader, const char* type, const char* shaderName)
{
	int success;
	char infoLog[1024];
	if (strcmp(type, "PROGRAM") != 0)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			LOGI("ERROR::SHADER_COMPILATION_ERROR of type: %s \n %s \n", type, infoLog);
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
			LOGI("ERROR::PROGRAM_LINKING_ERROR of type: %s \n %s \n", type, infoLog);
            LOGI("In shader %s\n", shaderName);
			ASSERT(false);
		}
	}
}

Shader shader_compile_from_file(const char* vertexFilepath, const char* fragmentFilepath)
{
	char* vertexCode;
	char* fragmentCode;

	vertexCode   = io_read_file(vertexFilepath);
	fragmentCode = io_read_file(fragmentFilepath);

	return shader_compile_shaders_from_source(vertexCode, fragmentCode, vertexFilepath, fragmentFilepath);
}

void shader_set_ints(Shader* shader, const char* name, int count, int* values)
{
	glUniform1iv(glGetUniformLocation(shader->ID, name), count, values);
}

void shader_set_mat4(Shader* shader, const char* name, mat4* value) 
{
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (GLfloat*)value);
}
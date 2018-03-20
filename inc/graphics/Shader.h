#pragma once
// #include <GLES2/gl.h>
#if defined(__EMSCRIPTEN__)
// #include <SDL2/SDL_opengles2.h>
#else
#include <glad/glad.h>
#endif

#include "../src/IO.c"

// #include <GLES3/gl3.h>
// #include <core/IO.h>
// #include <core/Log.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/type_ptr.hpp>

void checkCompileErrors(unsigned int shader, const char* type, const char* shaderName);

typedef struct Shader
{
	unsigned int ID;
} Shader;

void shader_use(Shader* s)
{
	glUseProgram(s->ID);
}

Shader shader_compile_shaders_from_source(const char * vertexSrc, const char * fragmentSrc,
	const char* vertexName, const char* fragmentName)
{
	Shader result;
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSrc, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX", vertexName);

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSrc, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT", fragmentName);

	// shader Program
	result.ID = glCreateProgram();
	glAttachShader(result.ID, vertex);
	glAttachShader(result.ID, fragment);
	glLinkProgram(result.ID);
	checkCompileErrors(result.ID, "PROGRAM", fragmentName);

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return result;
}

Shader shader_compile_shader_from_file(const char* vertexFilepath, const char* fragmentFilepath);
void shader_set_ints(Shader* shader, const char* name, int count, int* values);
void shader_set_mat4(Shader* shader, const char* name, mat4* value);






#if 0
void Shader_SetVector3f(const char* name, const glm::vec3 &value)
{
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}

void SetVector4f(const char* name, const vec4 &value)
{
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.w, value.h);
}
#if 1
void compileShaderFromFile(const char* vertexFilepath, const char* fragmentFilepath, bool modernVersion = false)
{
	std::string vertexCode;
	std::string fragmentCode;

#if !__EMSCRIPTEN__
	if (modernVersion)
	{
		const int additionalLen = 20;
		ASSERT(strlen(vertexFilepath) + additionalLen < 256); // overflow :(
		ASSERT(strlen(fragmentFilepath) + additionalLen < 256);
		char fragbuf[256];
		char verbuf[256];

		sprintf(verbuf, "%s_43", vertexFilepath);
		sprintf(fragbuf, "%s_43", fragmentFilepath);

		vertexCode = engine::io::ReadEntireFileToString(verbuf);
		fragmentCode = engine::io::ReadEntireFileToString(fragbuf);
	}
	else
#endif
	{
		vertexCode = engine::io::ReadEntireFileToString(vertexFilepath);
		fragmentCode = engine::io::ReadEntireFileToString(fragmentFilepath);
	}


	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	compileShadersFromSource(vShaderCode, fShaderCode, vertexFilepath, fragmentFilepath);
}
#endif

void compileShadersFromSource(const char * vertexSrc, const char * fragmentSrc,
	const char* vertexName, const char* fragmentName)
{
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSrc, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX", vertexName);

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSrc, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT", fragmentName);

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM", fragmentName);

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);


}

// activate the shader
// ------------------------------------------------------------------------
void use()
{
	glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void setInts(const std::string &name, int count, int* values) const
{
	glUniform1iv(glGetUniformLocation(ID, name.c_str()), count, values);
}

// ------------------------------------------------------------------------
void setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void setFloat2(const std::string &name, glm::vec2& value) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void setMat4(const std::string &name, glm::mat4 value) const
{
	float* ptr = glm::value_ptr(value);
	GLuint id = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(id, 1, GL_FALSE, ptr);
}

void setMat4(const unsigned int ID, glm::mat4 value) const
{
	glUniformMatrix4fv(ID, 1, GL_FALSE, glm::value_ptr(value));
}


unsigned int getLocation(const char* name)
{
	return glGetUniformLocation(ID, name);
}

void setFloat4(const char* name, const glm::vec4& vec4)
{
	glUniform4f(glGetUniformLocation(ID, name), vec4.x, vec4.y, vec4.z, vec4.w);
}

#endif
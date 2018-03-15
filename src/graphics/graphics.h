#pragma once

#include <chrono>
#include "../platform.h"
#include "graphicsF.h"
#include "shader.cpp"

constexpr int MAX_SPRITES = 100000;
struct VertexData
{
	float pos[MAX_SPRITES * 12];
	float uvs[MAX_SPRITES * 8];
	float ids[MAX_SPRITES * 4];
	unsigned int colors[MAX_SPRITES * 4];
	int textureSlots[32];
	unsigned int slotCount;
	unsigned int indicesCount;
};

struct SpriteBatch
{
	unsigned int VAO;
	unsigned int buffers[5];
	engine::Shader shader;
	VertexData vertexData;
};


EXPORT INIT_GAME(gameInit);
EXPORT UPDATE_GAME(gameUpdate);
EXPORT DRAW_GAME(gameDraw);


#define _DEBUG 0
#if defined(_DEBUG)
#define glCheckError()  glCheckError__(__FILE__, __LINE__) 
#else
#define glCheckError()
#endif

GLenum glCheckError__(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			// case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			// case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		LOGI("%s | %s (%i)\n", error.c_str(), file, line);
	}
	return errorCode;
}

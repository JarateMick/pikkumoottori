#pragma once

#include "../platform.h"
#include "graphicsF.h"
#include "shader.c"

#define MAX_SPRITES 500000
typedef struct 
{
	float pos[MAX_SPRITES * 16];
	float uvs[MAX_SPRITES * 8];
	float ids[MAX_SPRITES * 4];
	float rot[MAX_SPRITES * 4];
	unsigned int colors[MAX_SPRITES * 4];
	int textureSlots[32];
	unsigned int slotCount;
	unsigned int indicesCount;
} VertexData;

typedef struct 
{
	unsigned int VAO;
	unsigned int buffers[6];
	Shader shader;
	VertexData vertexData;
} SpriteBatch;


typedef struct 
{
	mat4 cameraMatrix;
	mat4 orthoMatrix;
	// glm::mat4 cameraMatrix;
	// glm::mat4 orthoMatrix;
} Camera2D;

typedef struct 
{

} GraphicsState;


EXPORT INIT_GAME(initGraphics);
EXPORT UPDATE_GAME(updateGraphics);
EXPORT DRAW_GAME(drawGraphics);


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
		char* error = "no";
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
		LOGI("%s | %s (%i)\n", error, file, line);
	}
	return errorCode;
}

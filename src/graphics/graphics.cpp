#define _CRT_SECURE_NO_WARNINGS 1
#include "graphics.h"

#include <glad/glad.h>
#include "../glad.c"

static SpriteBatch spriteBatch;

constexpr int MAX_SPRITES = 100000;
constexpr int POS = 0;
constexpr int UVS = 1;
constexpr int ID = 2;
constexpr int COLOR = 3;
constexpr int INDEX = 4;

void initSpriteBatch(SpriteBatch* sb)
{
	glGenVertexArrays(1, &sb->VAO);
	glGenBuffers(5, sb->buffers);

	glBindVertexArray(sb->VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 3, NULL, GL_DYNAMIC_DRAW); // 3 neliötä    4    pistettä            3     positionia      floatteina

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 2, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ID]);
	glVertexAttribPointer(2, 1, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 1, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(unsigned int) * 4 * 1, NULL, GL_DYNAMIC_DRAW);

	int indexBufferSize = MAX_SPRITES * 6 * sizeof(float);
	unsigned int* indices = (unsigned int)malloc(indexBufferSize);
	int index = 0;
	for (int i = 0; i < MAX_SPRITES * 6; i += 6)
	{
		indices[i] = index + 0;
		indices[i + 1] = index + 1;
		indices[i + 2] = index + 2;
		indices[i + 3] = index + 2;
		indices[i + 4] = index + 3;
		indices[i + 5] = index + 0;
		index += 4;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sb->buffers[INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_SPRITES * sizeof(GLuint) * 6, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	// glCheckError();


	// Init shader textures here
}

EXPORT INIT_GAME(initGame)
{
	if (!engine->context.initted)
	{
		LOGI("Graphics init\n");

		if (!gladLoadGL())
		{
			printf("failed to initaliaze GLAD\n");
			ASSERT(false);
		}

		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(114.f / 255.0f, 144.f / 255.0f, 154.f / 255.0f, 1.0f);

		// init sprite batch
		initSpriteBatch(&spriteBatch);

		engine->context.initted = true;
	}
}

EXPORT UPDATE_GAME(updateGame)
{
	//	LOGI("update \n");
}

EXPORT DRAW_GAME(drawGame)
{
	auto* c = &engine->context;
	if (c->updateViewPort)
	{
		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
	}

	glClear(GL_COLOR_BUFFER_BIT);
}

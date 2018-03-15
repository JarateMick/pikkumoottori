#define _CRT_SECURE_NO_WARNINGS 1
#include "graphics.h"

#include <glad/glad.h>
#include "../glad.c"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Imgui/imgui.h"


static SpriteBatch spriteBatch;

constexpr int POS = 0;
constexpr int UVS = 1;
constexpr int ID = 2;
constexpr int COLOR = 3;
constexpr int INDEX = 4;
constexpr int MAX_TEXTURES = 8;

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
	unsigned int* indices = (unsigned int*)malloc(indexBufferSize);
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

	sb->shader.compileShaderFromFile(
		"assets/shaders/spriteVert.txt",
		"assets/shaders/spriteFrag.txt");

	glCheckError();

	sb->shader.use();
	int values[8]{ 0, 1, 2, 3, 4, 5, 6, 7 };
	sb->shader.setInts("textures", 8, values);

	glCheckError();
}

void prepareBatch(Sprites* sprites, SpriteBatch* sb)
{
	int count = sprites->count;
	VertexData* vertexData = &sb->vertexData;
	vertexData->indicesCount = 6 * sprites->count;

	{
		float* __restrict positions = vertexData->pos;
		Vec2* __restrict pos = sprites->positions;
		Vec2* __restrict size = sprites->sizes;

		for (int i = 0; i < count * 12; i += 12)
		{
			*(positions + i + 0) = pos->x;
			*(positions + i + 1) = pos->y + size->y;
			*(positions + i + 2) = 0.0f;

			*(positions + i + 3) = pos->x;
			*(positions + i + 4) = pos->y;
			*(positions + i + 5) = 0.0f;

			*(positions + i + 6) = pos->x + size->x;
			*(positions + i + 7) = pos->y;
			*(positions + i + 8) = 0.0f;

			*(positions + i + 9) = pos->x + size->x;
			*(positions + i + 10) = pos->y + size->y;
			*(positions + i + 11) = 0.0f;

			++pos;
			++size;
		}
	}

	{
		vec4* __restrict uvs = sprites->uvs;
		float* __restrict uvsOut = vertexData->uvs;

		for (int i = 0; i < count * 8; i += 8)
		{
			*(uvsOut + i + 0) = uvs->x;
			*(uvsOut + i + 1) = uvs->y;

			*(uvsOut + i + 2) = uvs->x;
			*(uvsOut + i + 3) = uvs->y + uvs->h;

			*(uvsOut + i + 4) = uvs->x + uvs->w;
			*(uvsOut + i + 5) = uvs->y + uvs->h;

			*(uvsOut + i + 6) = uvs->x + uvs->w;
			*(uvsOut + i + 7) = uvs->y;

			++uvs;
		}
	}

	{
		vec4* __restrict color = sprites->colors;
		unsigned int* colorOut = vertexData->colors;

		for (int i = 0; i < count * 4; i += 4)
		{
			int r = int(color->x * 255.0f);
			int g = int(color->y * 255.0f);
			int b = int(color->w * 255.0f);
			int a = int(color->h * 255.0f);

			unsigned int c = (a << 24 | b << 16 | g << 8 | r);

			*(colorOut + i + 0) = c;
			*(colorOut + i + 1) = c;
			*(colorOut + i + 2) = c;
			*(colorOut + i + 3) = c;

			++color;
		}
	}

	{
		float* __restrict idsOut = vertexData->ids;
		int* __restrict ids = sprites->ids;

		for (int i = 0; i < ArrayCount(VertexData::textureSlots); ++i)
		{
			vertexData->textureSlots[i] = -1;
		}

		vertexData->slotCount = 0;

		for (int i = 0; i < count * 4; i += 4)
		{
			// get texture id
			float ts;
			bool found = false;
			for (int i = 0; i < MAX_TEXTURES; i++)
			{
				if (vertexData->textureSlots[i] == *ids)
				{
					ts = (float)(i + 1);
					found = true;
					break;
				}
			}

			if (!found)
			{
				vertexData->textureSlots[vertexData->slotCount] = *ids;
				(vertexData->slotCount)++;
				ts = (float)(vertexData->slotCount);
			}

			*(idsOut + i + 0) = ts;
			*(idsOut + i + 1) = ts;
			*(idsOut + i + 2) = ts;
			*(idsOut + i + 3) = ts;

			++ids;
		}
	}
}

void renderBatch(Sprites* sprites, SpriteBatch* sb, glm::mat4* cam)
{
	int count = sprites->count;
	VertexData* vertexData = &sb->vertexData;
	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertexData->pos, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertexData->uvs, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ID]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertexData->ids, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(unsigned int), vertexData->colors, GL_DYNAMIC_DRAW);
	glCheckError();

	sb->shader.use();
	sb->shader.setMat4("position", *cam);

	glBindVertexArray(sb->VAO);

	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (vertexData->textureSlots[i] == -1)
			break;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, vertexData->textureSlots[i]);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sb->buffers[INDEX]);
	glDrawElements(GL_TRIANGLES, vertexData->indicesCount, GL_UNSIGNED_INT, 0);

}

struct Texture2D
{
	int width, height;
	unsigned int ID;
	uint64_t writeTime;
};

static bool IsPowerOfTwo(unsigned long x)
{
	return (x & (x - 1)) == 0;
}

static Texture2D loadTexture(int width, int height, int bytesPerPixel, const unsigned char* pixels,
	bool clampToEdge = false, bool nearest = true)
{
	Texture2D value;

	ASSERT(bytesPerPixel == 3 || bytesPerPixel == 4);
	value.width = width;
	value.height = height;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &value.ID);
	glBindTexture(GL_TEXTURE_2D, value.ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (clampToEdge)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	GLuint fmt = bytesPerPixel == 3 ? GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, pixels);

	if (IsPowerOfTwo(width) && IsPowerOfTwo(height))
		glGenerateMipmap(GL_TEXTURE_2D);              // webGL wants power of two textures 
													  // for mipmaps.
	return value;
}

static Texture2D testTexture()
{
	unsigned char pixels[3] =  {
		255, 0, 255,
	};
	return loadTexture(1, 1, 3, pixels, false, false);
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

constexpr int BENCH_COUNT = 100000;
struct Entitys
{
	Vec2 pos[BENCH_COUNT];
	Vec2 size[BENCH_COUNT];
	Vec2 vel[BENCH_COUNT];
	int textureId[BENCH_COUNT];
	vec4 uvs[BENCH_COUNT];
	vec4 colors[BENCH_COUNT];
	unsigned int count;
};

EXPORT DRAW_GAME(drawGame)
{
	auto* c = &engine->context;
	if (c->updateViewPort)
	{
		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
	}

	glClear(GL_COLOR_BUFFER_BIT);

	static bool init = false;
	static Entitys* test = 0;
	static Texture2D texture2d = testTexture();
	if (!init)
	{
		test = new Entitys;
		init = true;

		int count = ArrayCount(Entitys::pos);
		const int RNG_MAX = 0xFFFFFFFF;
		for (int i = 0; i < count; ++i)
		{
			test->pos[i].x = rand() % (int)engine->windowDims.x;
			test->pos[i].y = rand() % (int)engine->windowDims.y;
		}

		for (int i = 0; i < count; ++i)
		{
			test->size[i] = { 20.f, 20.f };
			test->uvs[i] = { 0.f, 0.f, 1.f, 1.f };
			test->colors[i] = { 1.f, 1.f, 1.f, 1.f };
		}

		for (int i = 0; i < count; ++i)
		{
			test->textureId[i] = texture2d.ID;//load
		}

		for (int i = 0; i < count; ++i)
		{
			test->vel[i].x = (rand() % 1000) / 1000.f;
			test->vel[i].y = (rand() % 1000) / 1000.f;
		}

		c->sprites.colors = test->colors;
		c->sprites.ids = test->textureId;
		c->sprites.positions = test->pos;
		c->sprites.sizes = test->size;
		c->sprites.uvs = test->uvs;
		c->sprites.count = count;
	}

	for (int i = 0; i < BENCH_COUNT; ++i)
	{
		test->pos[i].x += test->vel[i].x;
		test->pos[i].y += test->vel[i].y;
	}

	for (int i = 0; i < BENCH_COUNT; ++i)
	{
		if (test->pos[i].x < 0 | test->pos[i].x > engine->windowDims.x)
		{
			test->vel[i].x = -test->vel[i].x;
			test->pos[i].x += test->vel[i].x;
		}

		if (test->pos[i].y < 0 | test->pos[i].y > engine->windowDims.y)
		{
			test->vel[i].y = -test->vel[i].y;
			test->pos[i].y += test->vel[i].y;
		}
	}

	prepareBatch(&c->sprites, &spriteBatch);

	auto mat = glm::ortho(0.0f, (float)engine->windowDims.x, 0.0f, (float)engine->windowDims.y);
	renderBatch(&c->sprites, &spriteBatch, &mat);
}

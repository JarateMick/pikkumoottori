#define _CRT_SECURE_NO_WARNINGS 1
#include "graphics.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#include "../glad.c"
#endif

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include "../Imgui/imgui.h"
// #include "../Imgui/imgui_impl_sdl_gl3.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"

static SpriteBatch spriteBatch;
static Camera2D camera;
static LineBatcher lineBatcher;

#define POS 0
#define UVS 1
#define B_ID 2
#define COLOR 3
#define ROTATION 4
#define INDEX 5
#define MAX_TEXTURES 8

static void initSpriteBatch(SpriteBatch* sb)
{
	glGenVertexArrays(1, &sb->VAO);
	glGenBuffers(ArrayCount(sb->buffers), sb->buffers);

	glBindVertexArray(sb->VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW); // 3 neliötä    4    pistettä            3     positionia      floatteina

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 2, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[B_ID]);
	glVertexAttribPointer(2, 1, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 1, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(unsigned int) * 4 * 1, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ROTATION]);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 1, NULL, GL_DYNAMIC_DRAW);


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
	free(indices); // you should use gl_map_buffer for this

	glBindVertexArray(0);


	sb->shader = shader_compile_from_file("assets/shaders/spriteVert.txt", "assets/shaders/spriteFrag.txt");

	glCheckError();

	shader_use(&sb->shader);
	int values[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	shader_set_ints(&sb->shader, "textures", 8, values);

	glCheckError();
}


static void initLineBatcher(LineBatcher* lineBatcher)
{
	glGenVertexArrays(1, &lineBatcher->VAO);

	glGenBuffers(1, &lineBatcher->posVBO);
	glGenBuffers(1, &lineBatcher->colorVBO);

	glBindVertexArray(lineBatcher->VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, lineBatcher->posVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, lineBatcher->colorVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

	lineBatcher->shader = shader_compile_from_file("assets/shaders/lines.vs", "assets/shaders/lines.fs");
	glCheckError();
}

static void renderLines(LineBatcher* lineBatcher, Lines* lines, mat4x4* cam)
{
	shader_use(&lineBatcher->shader);

//	shader_set_mat4(&lineBatcher->shader, "position", mat);
	glUniformMatrix4fv(glGetUniformLocation(lineBatcher->shader.ID, "position"), 1, GL_FALSE, (GLfloat*)cam);

	glBindBuffer(GL_ARRAY_BUFFER, lineBatcher->posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lines->count * 2, lines->lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, lineBatcher->colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint32) * lines->count, lines->colors, GL_DYNAMIC_DRAW);

	glBindVertexArray(lineBatcher->VAO);

	glDrawArrays(GL_LINES, 0, lines->count * 2);
	glCheckError();

	da_clear(lines->colors);
	da_clear(lines->lineVertices);
	lines->count = 0;
}

static void Geom_spritebatch_init(GeomSpritebatch* sb)
{
	glGenVertexArrays(1, &sb->VAO);
	glGenBuffers(ArrayCount(sb->buffers), sb->buffers);

	glBindVertexArray(sb->VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 2, NULL, GL_DYNAMIC_DRAW); // 3 neliötä    4    pistettä            3     positionia      floatteina

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[B_ID]);
	glVertexAttribPointer(1, 1, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ROTATION]);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	sb->shader = shader_compile_from_file_g("assets/shaders/geo_spriteVert.txt",
		"assets/shaders/geo_spriteFrag.txt",
		"assets/shaders/geo_spriteGeo.txt");

	glCheckError();

	shader_use(&sb->shader);
	int values[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	shader_set_ints(&sb->shader, "textures", 8, values);

	glCheckError();
}

static inline Vec2 rotate(Vec2 pos, float angle)
{
	Vec2 result;
	result.x = pos.x * cos(angle) - pos.y * sin(angle);
	result.y = pos.x * sin(angle) + pos.y * cos(angle);
	return result;
}

static inline void* mapBuffer(unsigned int type, unsigned int buffer)
{
	// glBindBuffer(type, buffer);
	// return glMapBuffer(type, GL_WRITE_ONLY);
	return 0;
}

static inline void* mapBufferRange(unsigned int type, unsigned int buffer, uint32 size)
{
	glBindBuffer(type, buffer);
	return glMapBufferRange(type, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}


#define NO_COPY 1
#define NO_MEMCPY 0
#define POS_SIZE  sizeof(float) * 4 * 4 * count
#define UV_SIZE   sizeof(float) * 4 * 2 * count
#define ID_SIZE   sizeof(float) * 4 * 1 * count
#define COLOR_SIZE  sizeof(uint32) * 4 * 1 * count
#define ROT_SIZE  sizeof(float) * 4 * 1 * count

void prepareBatch(Sprites* sprites, SpriteBatch* sb)
{
	// START_TIMING2();

	int count = sprites->count;
	VertexData* vertexData = &sb->vertexData;
	vertexData->indicesCount = 6 * sprites->count;

	{
		Vec2* __restrict pos = sprites->positions;
		Vec2* __restrict size = sprites->sizes;

		float* __restrict angle = sprites->rotation;

#if NO_COPY
		float* positions = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[POS], POS_SIZE);
		//mapBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);


#else
		float* positions = vertexData->pos;
#endif

		for (int i = 0; i < count * 16; i += 16)
		{
#if 0
			Vec2 half = *size / 2.f;

			Vec2 tl = { -half.x, half.y };
			Vec2 bl = { -half.x, -half.y };
			Vec2 br = { half.x, -half.y };
			Vec2 tr = { half.x, half.y };

			tl = rotate(tl, *angle) + half;
			bl = rotate(bl, *angle) + half;
			br = rotate(br, *angle) + half;
			tr = rotate(tr, *angle) + half;


			*(positions + i + 0) = pos->x + tl.x;
			*(positions + i + 1) = pos->y + tl.y;
			*(positions + i + 2) = 0.0f;
			// 3

			*(positions + i + 4) = pos->x + bl.x;
			*(positions + i + 5) = pos->y + bl.y;
			*(positions + i + 6) = 0.0f;
			// 7

			*(positions + i + 8) = pos->x + br.x;
			*(positions + i + 9) = pos->y + br.y;
			*(positions + i + 10) = 0.0f;
			// 11

			*(positions + i + 12) = pos->x + tr.x;
			*(positions + i + 13) = pos->y + tr.y;
			*(positions + i + 14) = 0.0f;
			//  15

			++angle;
#elif 0

			* (positions + i + 0) = pos->x;
			*(positions + i + 1) = pos->y + size->y;
			*(positions + i + 2) = 0.0f;

			*(positions + i + 4) = pos->x;
			*(positions + i + 5) = pos->y;
			*(positions + i + 5) = 0.0f;

			*(positions + i + 8) = pos->x + size->x;
			*(positions + i + 9) = pos->y;
			*(positions + i + 8) = 0.0f;

			*(positions + i + 12) = pos->x + size->x;
			*(positions + i + 13) = pos->y + size->y;
			*(positions + i + 11) = 0.0f;
#else

#if NO_MEMCPY
			Vec2 half = *size / 2.f;
			*(positions + i + 0) = pos->x;
			*(positions + i + 1) = pos->y; // +size->y;
			*(positions + i + 2) = -half.x;
			*(positions + i + 3) = half.y;

			*(positions + i + 4) = pos->x;
			*(positions + i + 5) = pos->y;
			*(positions + i + 6) = -half.x;
			*(positions + i + 7) = -half.y;

			*(positions + i + 8) = pos->x; // +size->x;
			*(positions + i + 9) = pos->y;
			*(positions + i + 10) = half.x;
			*(positions + i + 11) = -half.y;

			*(positions + i + 12) = pos->x; // +size->x;
			*(positions + i + 13) = pos->y; // +size->y;
			*(positions + i + 14) = half.x;
			*(positions + i + 15) = half.y;
#else
			Vec2 half = vec2_div(size, 2.f);

			memcpy(positions + i + 0, pos, sizeof(float) * 2);
			*(positions + i + 2) = -half.x;
			*(positions + i + 3) = half.y;

			memcpy(positions + i + 4, pos, sizeof(float) * 2);
			*(positions + i + 6) = -half.x;
			*(positions + i + 7) = -half.y;

			memcpy(positions + i + 8, pos, sizeof(float) * 2);
			*(positions + i + 10) = half.x;
			*(positions + i + 11) = -half.y;

			memcpy(positions + i + 12, pos, sizeof(float) * 2);
			//memcpy(positions + i + 14, &half, sizeof(float) * 2);
			*(positions + i + 14) = half.x;
			*(positions + i + 15) = half.y;
#endif

#endif
			++pos;
			++size;
		}

#if NO_COPY
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	{
		vec4* __restrict uvs = sprites->uvs;

#if NO_COPY
		// float* uvsOut = (float*)mapBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
		float* uvsOut = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[UVS], UV_SIZE);
#else
		float* __restrict uvsOut = vertexData->uvs;
#endif


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

#if NO_COPY
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	{
		vec4* __restrict color = sprites->colors;
#if NO_COPY
		// unsigned int* colorOut = (unsigned int*)mapBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
		unsigned int* colorOut = (unsigned int*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[COLOR], COLOR_SIZE);
#else
		unsigned int* colorOut = vertexData->colors;
#endif


		for (int i = 0; i < count * 4; i += 4)
		{
			int r = (int)(color->x * 255.0f);
			int g = (int)(color->y * 255.0f);
			int b = (int)(color->w * 255.0f);
			int a = (int)(color->h * 255.0f);

			unsigned int c = (a << 24 | b << 16 | g << 8 | r);

#if NO_MEMCPY
			*(colorOut + i + 0) = c;
			*(colorOut + i + 1) = c;
			*(colorOut + i + 2) = c;
			*(colorOut + i + 3) = c;
#else
			memcpy(colorOut + i + 0, &c, sizeof(float));
			memcpy(colorOut + i + 1, &c, sizeof(float));
			memcpy(colorOut + i + 2, &c, sizeof(float));
			memcpy(colorOut + i + 3, &c, sizeof(float));
#endif

			++color;
		}

#if NO_COPY
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	{
		float* s_rots = sprites->rotation;

#if NO_COPY
		// float* rot = (float*)mapBuffer(GL_ARRAY_BUFFER, sb->buffers[ROTATION]);
		float* rot = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[ROTATION], ROT_SIZE);
#else
		float* rot = vertexData->rot;
#endif

		for (int i = 0; i < count * 4; i += 4)
		{
#if NO_MEMCPY
			*(rot + i + 0) = *s_rots;
			*(rot + i + 1) = *s_rots;
			*(rot + i + 2) = *s_rots;
			*(rot + i + 3) = *s_rots;
#else
			memcpy(rot + i + 0, s_rots, sizeof(float));
			memcpy(rot + i + 1, s_rots, sizeof(float));
			memcpy(rot + i + 2, s_rots, sizeof(float));
			memcpy(rot + i + 3, s_rots, sizeof(float));
#endif
			++s_rots;
			// memcpy(rot + i, s_rots, 4 * sizeof(float));
		}
#if NO_COPY
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	glCheckError();

	{
		// float* __restrict idsOut = vertexData->ids;
		int* __restrict ids = sprites->ids;

		// float* idsOut = (float*)mapBuffer(GL_ARRAY_BUFFER, sb->buffers[B_ID]);
		float* idsOut = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[B_ID], ID_SIZE);

		for (int i = 0; i < ArrayCount(vertexData->textureSlots); ++i)
		{
			vertexData->textureSlots[i] = -1;
		}

		vertexData->slotCount = 0;

		for (int i = 0; i < count * 4; i += 4)
		{
			// get texture id
			float ts;
			bool32 found = false;
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

#if NO_MEMCPY
			*(idsOut + i + 0) = ts;
			*(idsOut + i + 1) = ts;
			*(idsOut + i + 2) = ts;
			*(idsOut + i + 3) = ts;
#else
			memcpy(idsOut + i + 0, &ts, sizeof(float));
			memcpy(idsOut + i + 1, &ts, sizeof(float));
			memcpy(idsOut + i + 2, &ts, sizeof(float));
			memcpy(idsOut + i + 3, &ts, sizeof(float));
#endif

			++ids;
		}
#if NO_COPY
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	// END_TIMING2();
}



#undef NO_COPY
#undef NO_MEMCPY
#undef POS_SIZE  
#undef UV_SIZE   
#undef ID_SIZE   
#undef COLOR_SIZE
#undef ROT_SIZE  

#define NO_COPY 1
#define NO_MEMCPY 0
#define POS_SIZE  sizeof(float) * 2 *  count
#define UV_SIZE   sizeof(float) *    count
#define ID_SIZE   sizeof(float) *    count
#define COLOR_SIZE  sizeof(uint32) * count
#define ROT_SIZE  sizeof(float)  *   count

void geom_prepareBatch(Sprites* sprites, GeomSpritebatch* sb)

{
	int count = sprites->count;
	LightVertexData* vertexData = &sb->vertexData;

	{
		Vec2* __restrict pos = sprites->positions;
		Vec2* __restrict size = sprites->sizes;

		float* __restrict angle = sprites->rotation;

		float* positions = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[POS], POS_SIZE);
		memcpy(positions, pos, POS_SIZE);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	{
		vec4* __restrict color = sprites->colors;
		unsigned int* colorOut = (unsigned int*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[COLOR], COLOR_SIZE);

		for (int i = 0; i < count; ++i)
		{
			int r = (int)(color->x * 255.0f);
			int g = (int)(color->y * 255.0f);
			int b = (int)(color->w * 255.0f);
			int a = (int)(color->h * 255.0f);

			unsigned int c = (a << 24 | b << 16 | g << 8 | r);
			memcpy(colorOut + i, &c, sizeof(float));
			++color;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	{
		float* s_rots = sprites->rotation;

		float* rot = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[ROTATION], ROT_SIZE);
		memcpy(rot, s_rots, sizeof(float) * count);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	{
		int* __restrict ids = sprites->ids;

		float* idsOut = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[B_ID], ID_SIZE);

		for (int i = 0; i < ArrayCount(vertexData->textureSlots); ++i)
		{
			vertexData->textureSlots[i] = -1;
		}

		vertexData->slotCount = 0;
		for (int i = 0; i < count; ++i)
		{
			// get texture id
			float ts;
			bool32 found = false;
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

			memcpy(idsOut + i + 0, &ts, sizeof(float));
			++ids;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	glCheckError();
}
#undef NO_MEMCPY
#undef POS_SIZE  
#undef UV_SIZE   
#undef ID_SIZE   
#undef COLOR_SIZE
#undef ROT_SIZE  

void Geom_renderBatch(Sprites* sprites, GeomSpritebatch* sb, mat4x4* cam)
{
	int count = sprites->count;
	LightVertexData* vertexData = &sb->vertexData;

	shader_use(&sb->shader);
	glUniformMatrix4fv(glGetUniformLocation(sb->shader.ID, "projection"), 1, GL_FALSE, (GLfloat*)cam);

	glBindVertexArray(sb->VAO);

	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (vertexData->textureSlots[i] == -1)
			break;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, vertexData->textureSlots[i]);
	}

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sb->buffers[INDEX]);
	// glPointSize(1.f);
	glDrawArrays(GL_POINTS, 0, count);
}

void renderBatch(Sprites* sprites, SpriteBatch* sb, mat4x4* cam)
{
	int count = sprites->count;
	VertexData* vertexData = &sb->vertexData;
#if !NO_COPY
	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4 * 4, vertexData->pos, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 2 * 4, vertexData->uvs, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[B_ID]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, vertexData->ids, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(unsigned int) * 4, vertexData->colors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ROTATION]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, vertexData->rot, GL_DYNAMIC_DRAW);
#endif

	shader_use(&sb->shader);
	// shader_set_mat4(&sb->shader, "position", cam);
	glUniformMatrix4fv(glGetUniformLocation(sb->shader.ID, "position"), 1, GL_FALSE, (GLfloat*)cam);
	// sb->shader.setMat4("position", *cam);

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

static bool32 IsPowerOfTwo(unsigned long x)
{
	return (x & (x - 1)) == 0;
}

static Texture2D loadTexture(int width, int height, int bytesPerPixel, const unsigned char* pixels,
	bool32 clampToEdge, bool32 nearest)
{
	Texture2D value;

	ASSERT(bytesPerPixel == 3 || bytesPerPixel == 4);
	value.width = width;
	value.height = height;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &value.ID);
	glBindTexture(GL_TEXTURE_2D, value.ID);


	if (!IsPowerOfTwo(width) || !IsPowerOfTwo(height))
		clampToEdge = true;


	clampToEdge = true;
	if (clampToEdge)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
	unsigned char pixels[3] = {
		255, 0, 255,
	};
	return loadTexture(1, 1, 3, pixels, false, false);
}

//enum TextureEnum
//{
//	Texture_infantry,
//	Texture_circle,
//	Texture_box,
//	Texture_count,
//};

//constexpr const char* texturePathBase = "assets/textures/";
//#define TEXTURES \
//(const char* const[Texture_count]) { \
//	[Texture_infantry] = "infantry.png", \
//	[Texture_circle]   = "circle.png", \
//	[Texture_box]      = "box", \
// }




const char* baseTexturePath = "assets/textures/";

static Texture2D loadTextureE(TextureEnum texture)
{
	const char* path = TextureEnumToStr[texture];
	char buffer[256];
	ASSERT(strlen(baseTexturePath) + strlen(path) < 256);
	strcpy(buffer, baseTexturePath);
	strcat(buffer, path);
	strcat(buffer, ".png");

	int w, h, nrChannels;
	unsigned char* data = stbi_load(buffer, &w, &h, &nrChannels, 0);
	if (data)
	{
		return loadTexture(w, h, nrChannels, data, true, true);
		stbi_image_free(data);
	}
	else
	{
		LOGI("failed to load image %s \n", buffer);
		ASSERT(false);
		return testTexture();
	}
}

// can you implement sorter XD
// shame that it 'leaks' and can't handle hotreloading textures XD

static Texture2D* getTexture(ResourceHolder* h, TextureEnum texture)
{
	if (!h->loaded[texture])
	{
		h->textures[texture] = loadTextureE(texture);
		h->loaded[texture] = true;
	}
	return &h->textures[texture];
}

static GeomSpritebatch geomSpriteBat;

void initCamera(Camera2D* cam, int screenW, int screenH);
EXPORT INIT_GAME(initGraphics)
{
	GraphicsContext* c = &engine->context;
	if (!engine->context.initted)
	{
		LOGI("Graphics init\n");

#ifndef __EMSCRIPTEN__
		if (!gladLoadGL())
		{
			printf("failed to initaliaze GLAD\n");
			ASSERT(false);
		}
#endif

		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(114.f / 255.0f, 144.f / 255.0f, 154.f / 255.0f, 1.0f);


		//ImGui::StyleColorsClassic();
		initCamera(&camera, (int)engine->windowDims.x, (int)engine->windowDims.y);
		engine->context.camera.scale = 1.f;

		stbi_set_flip_vertically_on_load(false);

		engine->context.initted = true;

#ifndef __EMSCRIPTEN__
		// ImGui::SetCurrentContext((ImGuiContext*)engine->imguiContext);
#endif

		// Init functions

		GraphicsFuncs* funcs = &c->funcs;
		funcs->getTexture = getTexture;

		initSpriteBatch(&spriteBatch);

		Geom_spritebatch_init(&geomSpriteBat);

		initLineBatcher(&lineBatcher);
	}
}


// tekstuurit jaavat inisoimattomaan tilaan kun dll hotreloadataan
// todo tekstuuri cache pitka kestoiseen muistiin!
EXPORT UPDATE_GAME(updateGraphics)
{
	GraphicsContext* c = &engine->context;

#if 0
	if (engine->controller.mouseDown)
	{
		Vec2 pos = engine->controller.mousePos;
		float halfX = 40.f;
		float halfY = 40.f;

		pos = convertScreenToWorld(&engine->context.camera, pos, &engine->windowDims);

		vec4 bounds = { pos.x - halfX, pos.y - halfY, pos.x + halfX, pos.y + halfY };
		GraphicsState* state = (GraphicsState*)mem->memory;
		Entitys* ents = &state->ents;

		for (int i = 0; i < BENCH_COUNT; ++i)
		{
			if (ents->pos[i].x > bounds.x && ents->pos[i].x < bounds.w &&
				ents->pos[i].y > bounds.y && ents->pos[i].y < bounds.h)
			{
				ents->colors[i] = { 0.f, 0.f, 0.f, 1.0f };
			}
		}
	}
#endif

	if (engine->reloadGraphics)
	{
		engine->reloadGraphics = false;

#ifndef __EMSCRIPTEN__
		if (!gladLoadGL())
		{
			printf("failed to initaliaze GLAD\n");
			ASSERT(false);
		}
#endif

		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(114.f / 255.0f, 144.f / 255.0f, 154.f / 255.0f, 1.0f);

		initSpriteBatch(&spriteBatch);


		/*	ResourceHolder* h = &c->resourceHolder;
			int oldIds[Texture_count];
			int newIds[Texture_count];
			for (int i = 0; i < Texture_count; ++i)
			{
				if (h->loaded[i])
				{
					oldIds[i] = h->textures[i].ID;
					h->loaded[i] = false;
					newIds[i] = getTexture(h, (TextureEnum)i)->ID;
				}
			}

			for (int i = 0; i < c->sprites.count; ++i)
			{
				if (c->sprites.ids[i] >= 0 && c->sprites.ids[i] < Texture_count)
				{
					int old = c->sprites.ids[i];
					c->sprites.ids[i] = newIds[old];
				}
			}*/

#if 0
		GraphicsState* state = (GraphicsState*)mem->memory;
		state->testTextures[0] = getTexture(Texture_box);
		state->testTextures[1] = getTexture(Texture_circle);
		state->testTextures[2] = getTexture(Texture_awesomeface);

		Entitys* ents = &state->ents;
		for (int i = 0; i < BENCH_COUNT; ++i)
		{
			ents->textureId[i] = getTexture(ents->textureNames[i])->ID;
		}
#endif

		initCamera(&camera, engine->windowDims.x, engine->windowDims.y);
	}
}

void initCamera(Camera2D* cam, int screenW, int screenH)
{
	// glm::ortho(0.0f, (float)screenW, 0.0f, (float)screenH);
	mat4x4_ortho(cam->orthoMatrix, 0.0f, (float)screenW, 0.f, (float)screenH, -1.f, 1.f);
}

void updateCamera(Camera2D* cam, CameraState* state, Vec2* screen)
{
	if (state->needUpdate)
	{
#if 1

		//glm::vec3 translate(-state->position.x + screen->x / 2, -state->position.y + screen->y / 2, 0.0f);  //centeroi cameran myösä
		// cam->cameraMatrix = glm::translate(cam->orthoMatrix, translate);

		mat4x4 trans;
		mat4x4_dup(trans, cam->orthoMatrix);
		mat4x4_translate_in_place(trans, -state->position.x + screen->x / 2, -state->position.y + screen->y / 2, 0.f);

		// kun yksi niin normaali eli kerrotaan yhdellä | jos 0.5 zoomattu ulos ja 2.o niiin zoom in

		// glm::vec3 scale(state->scale, state->scale, 0.0f);
		// cam->cameraMatrix = /* rotate * */ glm::scale(glm::mat4(1.0f), scale) * cam->cameraMatrix;
		//camera scale^^

		// mat4x4 scl;
		// mat4x4_identity(scl);
		// mat4x4_scale(scl, scl, state->scale);
		// mat4x4_mul(trans, scl, trans);

		mat4x4_scale(cam->cameraMatrix, trans, state->scale);

		// mat4x4_dup(cam->cameraMatrix, trans);//cam->orthoMatrix);
		state->needUpdate = false;
#endif
	}
}


EXPORT DRAW_GAME(drawGraphics)
{
	GraphicsContext* c = &engine->context;
	GraphicsState* state = (GraphicsState*)mem->memory;

	if (c->updateViewPort)
	{
		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
	}

	glClear(GL_COLOR_BUFFER_BIT);


	Vec2 vel = { 0 };
	float speed = 40.f * engine->dt;
	Controller* controller = &engine->controller;
	if (controller->cameraMovement[ctrl])
	{
		speed *= 5.f;
	}
	if (controller->cameraMovement[w])
	{
		vel.y += speed;
	}
	if (controller->cameraMovement[s])
	{
		vel.y -= speed;
	}
	if (controller->cameraMovement[d])
	{
		vel.x += speed;
	}
	if (controller->cameraMovement[a])
	{
		vel.x -= speed;
	}
	if (controller->cameraMovement[q])
	{
		zoom(&c->camera, 15.33f * engine->dt);
	}
	if (controller->cameraMovement[e])
	{
		zoom(&c->camera, -15.33f * engine->dt);
	}
	if (vel.x != 0.f || vel.y != 0)
		translate(&c->camera, &vel);


	// prepareBatch(&c->sprites, &spriteBatch);
	static Sprites testSprites;
	static int init = false;
	if (!init)
	{
#define INIT_ARRAY(arr, type, count) static type arr[count];

#define TEST_COUNT 600000
		static Vec2 p[TEST_COUNT];
		static Vec2 s[TEST_COUNT];
		static vec4 co[TEST_COUNT];
		static int ids[TEST_COUNT];
		static float rot[TEST_COUNT];
		static int count = TEST_COUNT;
		Texture2D* texID = getTexture(&c->resourceHolder, Texture_awesomeface);

		// static Texture2D* getTexture(ResourceHolder* h, TextureEnum texture)

		for (int i = 0; i < TEST_COUNT; ++i)
		{
			p[i].x = i * 3.f;
			co[i].x = 1.f;
			co[i].y = 1.f;
			co[i].w = 1.f;
			co[i].h = 1.f;
			ids[i] = texID->ID;
			rot[i] = 0.f;
		}

		testSprites.positions = p;
		testSprites.sizes = s;
		testSprites.colors = co;
		testSprites.ids = ids;
		testSprites.rotation = rot;
		testSprites.count = TEST_COUNT;
	}

	// c->sprites = testSprites;

	updateCamera(&camera, &c->camera, &engine->windowDims);


	static float counter = 0.f;
	counter += 0.1f * engine->dt;
	testSprites.positions[1].y = sin(counter) * 5.f;

	prepareBatch(&c->sprites, &spriteBatch);
	renderBatch(&c->sprites, &spriteBatch, &camera.cameraMatrix);

//	geom_prepareBatch(&c->sprites, &geomSpriteBat);
//	Geom_renderBatch(&c->sprites, &geomSpriteBat, &camera.cameraMatrix);

	Vec2 a = { 10.f, 20.f };
	Vec2 g = { 10.f, 30.f };

	drawLine(c, &a, &g, 0xFF00FFFF);

	drawLine(c, &a, &g, 0xFF00FFFF);

	renderLines(&lineBatcher, &c->lines, &camera.cameraMatrix);
}

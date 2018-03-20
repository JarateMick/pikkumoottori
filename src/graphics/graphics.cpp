#define _CRT_SECURE_NO_WARNINGS 1
#include "graphics.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#include "../glad.c"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_sdl_gl3.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"

static SpriteBatch spriteBatch;
static Camera2D camera;

constexpr int POS = 0;
constexpr int UVS = 1;
constexpr int ID = 2;
constexpr int COLOR = 3;
constexpr int ROTATION = 4;
constexpr int INDEX = 5;
constexpr int MAX_TEXTURES = 8;

void initSpriteBatch(SpriteBatch* sb)
{
	glGenVertexArrays(1, &sb->VAO);
	glGenBuffers(ArrayCount(SpriteBatch::buffers), sb->buffers);

	glBindVertexArray(sb->VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW); // 3 neli�t�    4    pistett�            3     positionia      floatteina

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(float) * 4 * 2, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ID]);
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

	sb->shader.compileShaderFromFile(
		"assets/shaders/spriteVert.txt",
		"assets/shaders/spriteFrag.txt");

	glCheckError();

	sb->shader.use();
	int values[8]{ 0, 1, 2, 3, 4, 5, 6, 7 };
	sb->shader.setInts("textures", 8, values);

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
constexpr uint32 POS_SIZE = sizeof(float) * 4 * 4 * MAX_SPRITES;
constexpr uint32 UV_SIZE  = sizeof(float) * 4 * 2 * MAX_SPRITES;
constexpr uint32 ID_SIZE  = sizeof(float) * 4 * 1 * MAX_SPRITES;
constexpr uint32 COLOR_SIZE = sizeof(uint32) * 4 * 1 * MAX_SPRITES;
constexpr uint32 ROT_SIZE = sizeof(float) * 4 * 1 * MAX_SPRITES;

void prepareBatch(Sprites* sprites, SpriteBatch* sb)
{
	START_TIMING2();

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
			Vec2 half = *size / 2.f;

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
			int r = int(color->x * 255.0f);
			int g = int(color->y * 255.0f);
			int b = int(color->w * 255.0f);
			int a = int(color->h * 255.0f);

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


	{
		// float* __restrict idsOut = vertexData->ids;
		int* __restrict ids = sprites->ids;

		// float* idsOut = (float*)mapBuffer(GL_ARRAY_BUFFER, sb->buffers[ID]);
		float* idsOut = (float*)mapBufferRange(GL_ARRAY_BUFFER, sb->buffers[ID], ID_SIZE);

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

	END_TIMING2();
}

void renderBatch(Sprites* sprites, SpriteBatch* sb, glm::mat4* cam)
{
	int count = sprites->count;
	VertexData* vertexData = &sb->vertexData;
#if !NO_COPY
	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[POS]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4 * 4, vertexData->pos, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[UVS]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 2 * 4, vertexData->uvs, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ID]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, vertexData->ids, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[COLOR]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(unsigned int) * 4, vertexData->colors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, sb->buffers[ROTATION]);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, vertexData->rot, GL_DYNAMIC_DRAW);
#endif
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

static Texture2D loadTexture(TextureEnum texture)
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
		return loadTexture(w, h, nrChannels, data);
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
static Texture2D* getTexture(TextureEnum texture)
{
	static Texture2D textureCache[Texture_count] = { 0 };
	static bool loaded[Texture_count] = { 0 };

	if (!loaded[texture])
	{
		textureCache[texture] = loadTexture(texture);
		loaded[texture] = true;
	}

	return &textureCache[texture];
}

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

		// init sprite batch
		initSpriteBatch(&spriteBatch);

		//ImGui::StyleColorsClassic();
		initCamera(&camera, (int)engine->windowDims.x, (int)engine->windowDims.y);
		engine->context.camera.scale = 1.f;

		stbi_set_flip_vertically_on_load(false);

		engine->context.initted = true;

#ifndef __EMSCRIPTEN__
		ImGui::SetCurrentContext((ImGuiContext*)engine->imguiContext);
#endif

		// Init functions

		auto* funcs = &c->funcs;
		funcs->getTexture = getTexture;
	}
}

EXPORT UPDATE_GAME(updateGraphics)
{
	auto* c = &engine->context;

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

		ImGui::SetCurrentContext((ImGuiContext*)engine->imguiContext);

		initSpriteBatch(&spriteBatch);

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
	cam->orthoMatrix = glm::ortho(0.0f, (float)screenW, 0.0f, (float)screenH);
}

void updateCamera(Camera2D* cam, CameraState* state, Vec2* screen)
{
	if (state->needUpdate)
	{
		glm::vec3 translate(-state->position.x + screen->x / 2, -state->position.y + screen->y / 2, 0.0f);  //centeroi cameran my�s�
		cam->cameraMatrix = glm::translate(cam->orthoMatrix, translate);

		//glm::mat4 matRoll = glm::mat4(1.0f);
		//glm::mat4 matPitch = glm::mat4(1.0f);
		//glm::mat4 matYaw = glm::mat4(1.0f);

		//matRoll  = glm::rotate(matRoll, angle, glm::vec3(0.f, 0.f, 1.0f));
		//matPitch = glm::rotate(matPitch, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		//matYaw   = glm::rotate(matYaw, yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		// glm::mat4 rotate = matRoll * matPitch * matYaw;

		// kun yksi niin normaali eli kerrotaan yhdell� | jos 0.5 zoomattu ulos ja 2.o niiin zoom in
		glm::vec3 scale(state->scale, state->scale, 0.0f);
		cam->cameraMatrix = /* rotate * */ glm::scale(glm::mat4(1.0f), scale) * cam->cameraMatrix;
		//camera scale^^
		state->needUpdate = false;
	}
}


EXPORT DRAW_GAME(drawGraphics)
{
	auto* c = &engine->context;
	GraphicsState* state = (GraphicsState*)mem->memory;

	if (c->updateViewPort)
	{
		glViewport(0, 0, engine->windowDims.x, engine->windowDims.y);
	}

	glClear(GL_COLOR_BUFFER_BIT);


	Vec2 vel = {};
	float speed = 40.f * engine->dt;
	auto* controller = &engine->controller;
	if (controller->cameraMovement[Controller::ctrl])
	{
		speed *= 5.f;
	}
	if (controller->cameraMovement[Controller::w])
	{
		vel.y += speed;
	}
	if (controller->cameraMovement[Controller::s])
	{
		vel.y -= speed;
	}
	if (controller->cameraMovement[Controller::d])
	{
		vel.x += speed;
	}
	if (controller->cameraMovement[Controller::a])
	{
		vel.x -= speed;
	}
	if (controller->cameraMovement[Controller::q])
	{
		zoom(&c->camera, 0.33f * engine->dt);
	}
	if (controller->cameraMovement[Controller::e])
	{
		zoom(&c->camera, -0.33f * engine->dt);
	}
	translate(&c->camera, vel);

	prepareBatch(&c->sprites, &spriteBatch);

	updateCamera(&camera, &c->camera, &engine->windowDims);
	renderBatch(&c->sprites, &spriteBatch, &camera.cameraMatrix);
}

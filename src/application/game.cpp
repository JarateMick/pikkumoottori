#define _CRT_SECURE_NO_WARNINGS 1
#include "game.h"


static Texture2D* (*getTexture)(TextureEnum texture);

void initWalls(GameState* state, Sprites* sprites, int start);

EXPORT INIT_GAME(initGame)
{
	LOGI("game init\n");
	GraphicsContext* c = &engine->context;
	zoom(&c->camera, 1.f);

	// init global funcs
	getTexture = c->funcs.getTexture;

	{
		GameState* state = (GameState*)mem->memory;
		Entitys* test = &state->ents;

		state->testTextures[0] = getTexture(Texture_box);
		state->testTextures[1] = getTexture(Texture_circle);
		state->testTextures[2] = getTexture(Texture_awesomeface);

		TextureEnum names[3] = { Texture_box, Texture_circle, Texture_awesomeface };

		int count = BENCH_COUNT; // ArrayCount(Entitys::pos);
		const int RNG_MAX = 0xFFFFFFFF;
		for (int i = 0; i < count; ++i)
		{
			test->pos[i].x = rand() % (int)engine->windowDims.x;
			test->pos[i].y = rand() % (int)engine->windowDims.y;
		}

		for (int i = 0; i < count; ++i)
		{
			test->size[i] = { 14.f, 14.f };
			test->uvs[i] = { 0.f, 0.f, 1.f, 1.f };
			test->colors[i] = { 1.f, 1.f, 1.f, 1.f };
			test->rotation[i] = i / 10.f;
		}

		int a = getTexture(Texture_box)->ID;
		int b = getTexture(Texture_circle)->ID;


		for (int i = 0; i < count; ++i)
		{
			int r = rand() % ArrayCount(GameState::testTextures);
			test->textureId[i] = state->testTextures[r]->ID;
			test->textureNames[i] = names[r];

			if (test->textureId[i] == a || test->textureId[i] == b)
			{
				vec4 randomColor{ (rand() % 255) / 255.f , (rand() % 255) / 255.f, (rand() % 255) / 255.f };
				randomColor.h = rand() % 120 / 255.f + 0.2f;
				test->colors[i] = randomColor;
			}
		}

		for (int i = 0; i < count; ++i)
		{
			test->vel[i].x = (rand() % 100) / 100.f;
			test->vel[i].y = (rand() % 100) / 100.f;
		}

		// c->sprites.

		c->sprites.colors = test->colors;
		c->sprites.ids = test->textureId;
		c->sprites.positions = test->pos;
		c->sprites.sizes = test->size;
		c->sprites.uvs = test->uvs;
		c->sprites.rotation = test->rotation;
		c->sprites.count = count;

		test->bounds.x = 0.f;
		test->bounds.y = 0.f;
		test->bounds.w = engine->windowDims.x * 1.f;
		test->bounds.h = engine->windowDims.y * 1.f;

		initWalls(state, &c->sprites, BENCH_COUNT);
	}
}

static void makeSprite(Sprites* s, Vec2 pos, Vec2 size, int textureID)
{
	int id = s->count;
	s->positions[id] = pos;
	s->sizes[id] = size;
	s->colors[id] = { 1.f, 1.f, 1.f, 1.f };
	s->ids[id] = textureID;
	s->rotation[id] = 0.f;
	s->uvs[id] = { 0.f, 0.f, 1.0f, 1.0f };
	s->count += 1;
}

// bottom left is origo
static void makeWall(Sprites* s, Vec2 pos, Vec2 size, int id)
{
	pos += size / 2.f;
	makeSprite(s, pos, size, id);
}

static void initWalls(GameState* state, Sprites* s, int startIndex)
{
	auto texture = getTexture(Texture_box);

	// bottom
	makeWall(s, { 0.f, 0.f },   { 10.f, 100.f, }, texture->ID);
	// right
	makeWall(s, { 100.f - 10.f, 0.f }, { 10.f, 100.f, }, texture->ID);
	// top
	makeWall(s, { 0.f, 100.f - 10.f }, { 100.f, 10.f, }, texture->ID);
	// left
	makeWall(s, { 0.f, 0.f },   { 100.f, 10.f, }, texture->ID);
	
	// makeWall(s, { 0.f, 0.f },   { 100.f, 10.f, }, texture->ID);
	// makeWall(s, { 0.f, 100.f }, { 100.f, 10.f, }, texture->ID);
}

EXPORT UPDATE_GAME(updateGame)
{
	GraphicsContext* c = &engine->context;
	auto sprites = c->sprites;

	GameState* gameState = (GameState*)mem->memory;
	Entitys* test = &gameState->ents;

	float unitSpeed = 100 * engine->dt;
	for (int i = 0; i < BENCH_COUNT; ++i)
	{
		test->pos[i].x += test->vel[i].x * unitSpeed;
		test->pos[i].y += test->vel[i].y * unitSpeed;
	}

	for (int i = 0; i < BENCH_COUNT; ++i)
	{
		if (test->pos[i].x < test->bounds.x | test->pos[i].x > test->bounds.w)
		{
			test->vel[i].x = -test->vel[i].x;
			test->pos[i].x = test->pos[i].x < test->bounds.x ? test->bounds.x : test->bounds.w - 1.f;
		}

		if (test->pos[i].y < test->bounds.y | test->pos[i].y > test->bounds.h)
		{
			test->vel[i].y = -test->vel[i].y;
			test->pos[i].y = test->pos[i].y < test->bounds.y ? test->bounds.y : test->bounds.h - 1.f;
		}
	}

	for (int i = 0; i < BENCH_COUNT / 2; ++i)
	{
		test->rotation[i] += test->vel[i].x;
	}
	for (int i = BENCH_COUNT / 2; i < BENCH_COUNT; ++i)
	{
		test->rotation[i] -= test->vel[i].x;
	}
}


EXPORT DRAW_GAME(drawGame)
{
	GameState* gameState = (GameState*)mem->memory;
	//	LOGI("rEntitys* test = &state->ents;
#ifndef __EMSCRIPTEN__
	// ImGui::DragFloat2("bounds x, y", (&test->bounds.x));
	// ImGui::DragFloat2("bounds w, h", (&test->bounds.w));
#endif

	/*for (int i = BENCH_COUNT; i < BENCH_COUNT + 100; ++i)
	{
		gameState->ents.pos[i] = { i * 2.f, 0.f };
	}*/
}

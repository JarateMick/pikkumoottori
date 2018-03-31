#define _CRT_SECURE_NO_WARNINGS 1
#include "game.h"

void initWalls(GameState* state, Sprites* sprites, int texId, int start);

EXPORT INIT_GAME(initGame)
{
	LOGI("game init\n");

	GraphicsContext* c = &engine->context;
	// zoom(&c->camera, 1.f);

	{
		c->camera.scale = 1.f; // default scale
		c->camera.needUpdate = true;
		// c-.camera.position = 
	}

	getTexture = c->funcs.getTexture;

	GameState* state = (GameState*)mem->memory;
	memset(state, 0, sizeof(GameState));

	Entitys* test = &state->ents;
	state->count = PARTICLE_COUNT;


	initializeParticles(state->particles, state->count);

#if 1

	{
		ResourceHolder* h = &c->resourceHolder;
		state->testTextures[0] = getTexture(h, Texture_circle);
		state->testTextures[1] = getTexture(h, Texture_awesomeface);
		state->testTextures[2] = getTexture(h, Texture_box);

		TextureEnum names[3] = { Texture_box, Texture_circle, Texture_awesomeface };

		int count = PARTICLE_COUNT; // ArrayCount(Entitys::pos);

		const int RNG_MAX = 0xFFFFFFFF;
		for (int i = 0; i < count; ++i)
		{
			test->pos[i].x = rand() % (int)engine->windowDims.x;
			test->pos[i].y = rand() % (int)engine->windowDims.y;
		}

		for (int i = 0; i < count; ++i)
		{
			test->size[i] = V2(10.f, 10.f);
			test->uvs[i] = V4(0.f, 0.f, 1.f, 1.f);
			test->colors[i] = V4(1.f, 1.f, 1.f, 1.f);
			test->rotation[i] = i / 10.f;
		}

		int a = getTexture(h, Texture_box)->ID;
		int b = getTexture(h, Texture_circle)->ID;


		for (int i = 0; i < count; ++i)
		{
			int r = rand() % 1;
			test->textureId[i] = state->testTextures[r]->ID;
			test->textureNames[i] = names[r];

			if (test->textureId[i] == a || test->textureId[i] == b)
			{
				vec4 randomColor = { (rand() % 255) / 255.f , (rand() % 255) / 255.f, (rand() % 255) / 255.f };
				randomColor.h = rand() % 120 / 255.f + 0.2f;
				test->colors[i] = randomColor;
			}
		}

		for (int i = 0; i < count; ++i)
		{
			test->vel[i].x = (rand() % 100) / 100.f;
			test->vel[i].y = (rand() % 100) / 100.f;
		}

		// c->sprites.;

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

		// initWalls(state, &c->sprites, a, BENCH_COUNT);
	}


	testPhysicsBodies(state, c, 5);

#endif
}

#if 1
static void makeSprite(Sprites* s, Vec2 pos, Vec2 size, int textureID)
{
	int id = s->count;
	s->positions[id] = pos;
	s->sizes[id] = size;
	s->colors[id] = V4(1.f, 1.f, 1.f, 1.f);
	s->ids[id] = textureID;
	s->rotation[id] = 0.f;
	s->uvs[id] = V4(0.f, 0.f, 1.0f, 1.0f);
	s->count += 1;
}

// bottom left is origo
static void makeWall(Sprites* s, Vec2 pos, Vec2 size, int id)
{
	// pos += size / 2.f;
	vec2 translate = vec2_div(&size, 2.f);
	pos = vec2_addv(&pos, &translate);
	makeSprite(s, pos, size, id);
}

static void initWalls(GameState* state, Sprites* s, int texId, int startIndex)
{
	// bottom
	makeWall(s, V2(0.f, 0.f), V2(10.f, 100.f), texId);
	// right
	makeWall(s, V2(100.f - 10.f, 0.f), V2(10.f, 100.f), texId);
	// top;
	makeWall(s, V2(0.f, 100.f - 10.f), V2(100.f, 10.f), texId);
	// left
	makeWall(s, V2(0.f, 0.f), V2(100.f, 10.f), texId);

	// makeWall(s, { 0.f, 0.f },   { 100.f, 10.f, }, texture->ID);
	// makeWall(s, { 0.f, 100.f }, { 100.f, 10.f, }, texture->ID);
}
#endif

EXPORT UPDATE_GAME(updateGame)
{
	// GraphicsContext* c = &engine->context;
	// auto sprites = c->sprites;
	GameState* gameState = (GameState*)mem->memory;
	Entitys* test = &gameState->ents;


	getTexture = engine->context.funcs.getTexture; // TODO

#if 0
	if (engine->runToFrame > engine->currentFrame || engine->freeRun)
	{

		// hashParticles(gameState, gameState->particles);
		updateParticles(gameState->particles, gameState->count, engine->dt);
		// doParticleCollision(gameState, engine->dt);

		// hacks

		++engine->currentFrame;
	}

	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		gameState->ents.pos[i] = gameState->particles[i].position;
	}
#endif



	
	updateBodies(gameState, &engine->context, engine->controller.mouseWorldPos, engine->dt);

#if 0

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
#endif
}


EXPORT DRAW_GAME(drawGame)
{
	GameState* gameState = (GameState*)mem->memory;
	//	LOGI("rEntitys* test = &state->ents;
#ifndef __EMSCRIPTEN__
	// ImGui::DragFloat2("bounds x, y", (&test->bounds.x));
	// ImGui::DragFloat2("bounds w, h", (&test->bounds.w));
#endif


	drawBodies(&engine->context ,gameState, 10);

}



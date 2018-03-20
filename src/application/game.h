#pragma once

// #include <chrono>

#include <math.h>
#include <stdio.h>
#include "../platform.h"


// struct Particle
// {
// };

#define BENCH_COUNT 999
#define ENTS_COUNT 20000
typedef struct 
{
	Vec2 pos[ENTS_COUNT];
	Vec2 size[ENTS_COUNT];
	Vec2 vel[ENTS_COUNT];

#if 0
	TextureEnum textureNames[ENTS_COUNT];
	int textureId[ENTS_COUNT];
	vec4 uvs[ENTS_COUNT];
	vec4 colors[ENTS_COUNT];
	float rotation[ENTS_COUNT];
	unsigned int count;

	vec4 bounds{};
#endif
} Entitys;

typedef struct 
{
	Entitys ents;
#if 0
	Texture2D* testTextures[3];
#endif
} GameState;

EXPORT INIT_GAME(initGame);
EXPORT UPDATE_GAME(updateGame);
EXPORT DRAW_GAME(drawGame);

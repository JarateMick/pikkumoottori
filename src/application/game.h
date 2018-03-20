#pragma once

// #include <chrono>

#include <cmath>
#include <stdio.h>
#include "../platform.h"


// struct Particle
// {
// };

constexpr int BENCH_COUNT = 100;
constexpr int ENTS_COUNT = 20000;
struct Entitys
{
	Vec2 pos[ENTS_COUNT];
	Vec2 size[ENTS_COUNT];
	Vec2 vel[ENTS_COUNT];

	TextureEnum textureNames[ENTS_COUNT];
	int textureId[ENTS_COUNT];
	vec4 uvs[ENTS_COUNT];
	vec4 colors[ENTS_COUNT];
	float rotation[ENTS_COUNT];
	unsigned int count;

	vec4 bounds{};
};

struct GameState
{
	Entitys ents;
	Texture2D* testTextures[3];
};

EXPORT INIT_GAME(initGame);
EXPORT UPDATE_GAME(updateGame);
EXPORT DRAW_GAME(drawGame);

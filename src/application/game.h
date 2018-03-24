#pragma once

// #include <chrono>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <3rdparty/stretchy_buffer.h>

#include "../platform.h"

#define PARTICLE_COUNT 1000
#define GRAVITY 9.81f
#define RADIUS 5.f
#define SPEED 40.f
typedef struct
{
	Vec2  position;
	Vec2  velocity;
	float mass;
} Particle;

#define BENCH_COUNT 1000
#define ENTS_COUNT 10000
typedef struct
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

	vec4 bounds;
} Entitys;

#define GRID_SIZE_X 10
#define GRID_SIZE_Y 10
#define GRID_SIZE GRID_SIZE_X * GRID_SIZE_Y
#define CELL_SIZE 80.f

typedef struct
{
	Entitys ents;
	Texture2D* testTextures[3];

	Particle   particles[1000];
	Particle** particlesHash[GRID_SIZE];

	int count;
} GameState;

static inline Vec2 V2(float x, float y)
{
	Vec2 result = { x, y };
	return result;
}

static inline vec3 V3(float x, float y, float z)
{
	vec3 result = { x, y, z };
	return result;
}

static inline vec4 V4(float x, float y, float z, float w)
{
	vec4 result = { x, y, z, w };
	return result;
}

static Vec2 randomVec2(int maxX, int maxY)
{
	return V2(rand() % maxX, rand() % maxY);
}

void initializeParticles(Particle* particles, int count)
{
	for (int i = 0; i < count; ++i)
	{
		particles[i].mass = 1.f;
		particles[i].position = randomVec2(800, 800);
		particles[i].velocity = randomVec2(80, 80);
	}

	// test->pos[i].x = rand() % (int)engine->windowDims.x;
	// test->pos[i].y = rand() % (int)engine->windowDims.y;
}

static inline void tickParticle(Vec2* pos, Vec2* vel, float dt)
{
	vec2 dvel = vec2_mul(vel, dt);
	vec2_add_v(pos, pos, &dvel);
}

void hashParticles(GameState* state, Particle* particles)
{
	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		Particle* current = &particles[i];

		int x = (int)current->position.x / CELL_SIZE;
		int y = (int)current->position.y / CELL_SIZE;

		int index = GRID_SIZE_X * y + x;

		// TODO: fix
		if (index > 0 && index < GRID_SIZE)
//			sb_push(state->particlesHash[index], current);
	}
}

void doParticleCollision()
{
}

void updateParticles(Particle* particles, int count, float dt)
{
	// check collisions



	for (int i = 0; i < count; ++i)
	{
		tickParticle(&particles[i].position, &particles[i].velocity, dt);
	}

	vec4 bounds = { 0.f, 0.f, 800.f, 800.f };
	for (int i = 0; i < count; ++i)
	{
		Particle* a = &particles[i];
		Vec2 realPosA = vec2_add(&a->position, -RADIUS);

		for (int j = i + 1; j < count; ++j)
		{
			Particle* b = &particles[j];

			Vec2 realPosB = vec2_add(&b->position, -RADIUS);

			Vec2 distVec = vec2_subv(&realPosA, &realPosB);

			float len = vec2_len(&distVec);

			if (len < RADIUS + RADIUS)
			{
				// collides // maybe offset

				vec2_normalizeInPlace(&distVec);

				a->velocity.x = distVec.x * SPEED;
				a->velocity.y = distVec.y * SPEED;

				b->velocity.x = -distVec.x * SPEED;
				b->velocity.y = -distVec.y * SPEED;

				// tickParticle(&a->position, &a->velocity, dt * 2.f);
				// tickParticle(&b->position, &b->velocity, dt * 2.f);

			}
		}
	}

	for (int i = 0; i < count; ++i)
	{
		if (particles[i].position.x < bounds.x || particles[i].position.x > bounds.w)
		{
			particles[i].velocity.x = -particles[i].velocity.x;
			particles[i].position.x = particles[i].position.x < bounds.x ? bounds.x : bounds.w - 1.f;
		}

		if (particles[i].position.y < bounds.y || particles[i].position.y > bounds.h)
		{
			particles[i].velocity.y = -particles[i].velocity.y;
			particles[i].position.y = particles[i].position.y < bounds.y ? bounds.y : bounds.h - 1.f;
		}
	}

}


EXPORT INIT_GAME(initGame);
EXPORT UPDATE_GAME(updateGame);
EXPORT DRAW_GAME(drawGame);

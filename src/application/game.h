#pragma once

// #include <chrono>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RF_DARRAY_IMPLEMENTATION
#define RF_DARRAY_SHORT_NAMES
#include <3rdparty/rf_darray.h>

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

#define BENCH_COUNT 3000
#define ENTS_COUNT 100000
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

#define GRID_SIZE_X 20
#define GRID_SIZE_Y 20
#define GRID_SIZE GRID_SIZE_X * GRID_SIZE_Y
#define CELL_SIZE 40.f

typedef struct
{
	Particle* ptr;
} ParticlePtr;

typedef struct
{
	Entitys ents;
	Texture2D* testTextures[3];

	Particle     particles[PARTICLE_COUNT];
	ParticlePtr* particlesHash[GRID_SIZE];

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
	return V2(rand() % maxX - maxX / 2, rand() % maxY - maxY / 2);
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

typedef struct
{
	int x, y;
} vec2i;

static inline vec2i V2i(int x, int y)
{
	vec2i result = { x, y };
	return result;
}

void hashParticles(GameState* state, Particle* particles)
{
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		da_clear(state->particlesHash[i]);
	}

	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		Particle* current = &particles[i];

		int x = (int)current->position.x / CELL_SIZE;
		int y = (int)current->position.y / CELL_SIZE;

		vec2i grids[5];
		grids[0] = V2i(x - 1, y - 1);
		grids[1] = V2i(x, y - 1);
		grids[2] = V2i(x - 1, y);
		grids[3] = V2i(x, y);
		grids[4] = V2i(x - 1, y + 1);

		for (int j = 0; j < 5; ++j)
		{
			int index = GRID_SIZE_X * grids[j].y + grids[j].x;

			ParticlePtr ptr;
			ptr.ptr = current;

			// TODO: fix
			if (index >= 0 && index < GRID_SIZE)
				da_push(state->particlesHash[index], ptr);
		}
	}
}

void doParticleCollision(GameState* state, float dt)
{
	ParticlePtr** hash = state->particlesHash;
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		ParticlePtr* array = hash[i];
		if (array)
		{
			int count = da_size(array);
			for (int j = 0; j < count; ++j)
			{
				Particle* a = (array + j)->ptr;
				Vec2 realPosA = vec2_add(&a->position, -RADIUS);
				for (int k = j + 1; k < count; ++k)
				{
					Particle* b = (array + k)->ptr;

					Vec2 realPosB = vec2_add(&b->position, -RADIUS);
					Vec2 distVec = vec2_subv(&realPosA, &realPosB);

					// laita squared distance
					float len = vec2_len(&distVec);
					if (len < RADIUS + RADIUS)
					{
						// collides // maybe offset

						vec2_normalizeInPlace(&distVec);

						a->velocity.x = distVec.x * SPEED;
						a->velocity.y = distVec.y * SPEED;

						b->velocity.x = -distVec.x * SPEED;
						b->velocity.y = -distVec.y * SPEED;

						tickParticle(&a->position, &a->velocity, dt * 0.5f);
						tickParticle(&b->position, &b->velocity, dt * 0.5f);
					}
				}
			}
		}
	}

}
#if 0
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
#endif

void updateParticles(Particle* particles, int count, float dt)
{
	// check collisions
	for (int i = 0; i < count; ++i)
	{
		tickParticle(&particles[i].position, &particles[i].velocity, dt);
	}


	vec4 bounds = { 0.f, 0.f, 800.f, 800.f };

	// screen bounds
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

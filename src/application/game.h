#pragma once

// #include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RF_DARRAY_IMPLEMENTATION
#define RF_DARRAY_SHORT_NAMES
#include <3rdparty/rf_darray.h>

#include "../platform.h"

#define WALL_COLLISION_E 0.5f
#define PARTICLE_COUNT 200
// #define GRAVITY -9.81f / 2.f
#define GRAVITY 12000*-9.8f
#define RADIUS 5.f
#define SPEED 40.f

#ifndef __EMSCRIPTEN__
static Texture2D* (*getTexture)(ResourceHolder* h, TextureEnum texture);
#endif

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

typedef struct
{
	Vec2  position;
	Vec2  acceleration;
	Vec2  velocity;
	float mass;

	float density;
	float pressure;
} Particle;

#define BENCH_COUNT 200
#define ENTS_COUNT  300
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

typedef struct GameState GameState;
#include "../Physics.h"
struct GameState
{
	Entitys ents;
	Texture2D* testTextures[3];

	Particle     particles[PARTICLE_COUNT];
	ParticlePtr* particlesHash[GRID_SIZE];

	PhysicsBodies bodies[100];
	PhysicsController physicsControls;
	int count;
};
#include "../Physics.c"

// F = m*g
// f = ma

// f = F
// f = mg = F =  ma
// mg = ma 
// g = a

// forceY = mass * gravity;
// accelerationY = forceY / mass;
// velocity = velocity + acceleration * dt

// net force:
//   g + wind + 'flocking'
//   'table resist gravity' -g

// Floor collision: 
//   V
//   perfect velocity: E = 1.f
//   -E * V

// angled:
//  V' = Vparaller - E*Vperp

// 



static Vec2 randomVec2(int maxX, int maxY)
{
	return V2(rand() % maxX, rand() % maxY);
}

void initializeParticles(Particle* particles, int count)
{
#if 0
#else

	for (int i = 0; i < count; ++i)
	{
		particles[i].mass = 1.f;
		particles[i].position = V2(i * 2.f, i * RADIUS * 2); // randomVec2(800, 800);
		particles[i].velocity = V2(0, 0); //randomVec2(0, 0);
	}
#endif

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

						// dist vec on perp
						// a = 

						vec2_normalizeInPlace(&distVec);
						vec2_mul(&distVec, RADIUS / 2);

						vec2_add_v(&a->position, &a->position, &distVec);
						vec2_sub_v(&b->position, &b->position, &distVec);

						// float deepNess = 
						// a->position

						// a->velocity.x = distVec.x * SPEED;
						// a->velocity.y = distVec.y * SPEED;

						// b->velocity.x = -distVec.x * SPEED;
						// b->velocity.y = -distVec.y * SPEED;

						// tickParticle(&a->position, &a->velocity, dt * 0.5f);
						// tickParticle(&b->position, &b->velocity, dt * 0.5f);
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

#define REST_DENS 1000.f
#define GAS_CONST 2000.f

#define MASS 65.f
#define VISC 250.f

// smoothing kernels
#define H 16.f
#define HSQ H * H
#define POLY6 315.f / (65.f * M_PI * pow(H, 9.f))
#define SPIKY_GRAD -45.f / (M_PI * pow(H, 6.f))
#define VISC_LAP 45.f / (M_PI * pow(H, 6.f))

// for each:
// 
// 


void computeDensityPressure(Particle* particles, int count)
{
	for (int i = 0; i < count; ++i)
	{
		Particle* a = particles + i;
		a->density = 0.f;

		for (int j = 0; j < count; ++j)
		{
			Particle* b = particles + j;

			// calc dist
			Vec2 distVec = vec2_subv(&a->position, &b->position);	

			// float len = vec2_len(&distVec);
			float len = distVec.x * distVec.x + distVec.y * distVec.y;

			if (len < HSQ)
			{
				a->density += MASS * POLY6 * pow(HSQ - len, 3.f);
			}
		}
		a->pressure = GAS_CONST * (a->density - REST_DENS);
	}
}

// pressure p
// den rho

void calculateForces(Particle* particles, int count)
{
	int first = true;
	for (int i = 0; i < count; ++i)
	{
		Vec2 fpress = { 0.f, 0.f };
		Vec2 fvisc = { 0.f, 0.f };

		Particle* a = particles + i;
		for (int j = 0; j < count; ++j)
		{
			if (i == j)
				continue;

			Particle* b = particles + j;
			Vec2 distVec = vec2_subv(&a->position, &b->position);
			float len = vec2_len(&distVec);

			if (len < H)
			{
				vec2_normalizeInPlace(&distVec);

				distVec.x = -distVec.x;
				distVec.y = -distVec.y;

				// pressure contribution
				// float press = MASS * (a->pressure + b->pressure) 
				// float divider = (2.f * b->density) * SPIKY_GRAD * pow(H - len, 2.f);

				// force contribution


				vec2_mul_s(&distVec, &distVec, MASS * (a->pressure + b->pressure));
				vec2_div_s(&distVec, &distVec, (2.f * b->density));
				vec2_mul_s(&distVec, &distVec, SPIKY_GRAD * pow(H - len, 2.f));

				vec2_add_v(&fpress, &fpress, &distVec);

				// float fvisc = VISC * MASS * 

				vec2 velDist;
				vec2_sub_v(&velDist, &b->velocity, &a->velocity);

				vec2_mul_s(&velDist, &velDist, VISC * MASS);
				vec2_div_s(&velDist, &velDist, b->density);
				vec2_mul_s(&velDist, &velDist, VISC_LAP * (H - len));


				vec2_add_v(&fvisc, &fvisc, &velDist);
			}
		}

		// gravity
		float grav = GRAVITY * a->density;
		fpress.y += grav;
		vec2_add_v(&fpress, &fpress, &fvisc);
		a->acceleration = fpress;

		if (first)
		{
			first = false;
			 printf("f: %f %f  fpress: %f %f     fvisc %f %f \n", a->acceleration.x, a->acceleration.y,
				 fpress.x, fpress.y, fvisc.x, fvisc.y);
		}
	}
}


void updateParticles(Particle* particles, int count, float dt)
{
	
	// vec2 c = { 10.f, 10.f };
// #include "../test123.h"


	computeDensityPressure(particles, count);
	calculateForces(particles, count);

	// check collisions
	for (int i = 0; i < count; ++i)
	{
		// gravity
		// float m = particles[i].mass;
		// float forceY = m * GRAVITY;
		// float accY = forceY / m;
		// particles[i].velocity.y = particles[i].velocity.y + accY;

		dt =  dt * 0.5f;

		particles[i].acceleration = vec2_mul(&particles[i].acceleration, dt);
		particles[i].acceleration = vec2_div(&particles[i].acceleration, particles[i].density);
		vec2_add_v(&particles[i].velocity, &particles[i].velocity, &particles[i].acceleration);

		tickParticle(&particles[i].position, &particles[i].velocity, dt);
	}


	vec4 bounds = { 0.f, 0.f, 800.f, 800.f };

	// screen bounds
	for (int i = 0; i < count; ++i)
	{
		if (particles[i].position.x < bounds.x || particles[i].position.x > bounds.w)
		{
			particles[i].velocity.x = -particles[i].velocity.x * WALL_COLLISION_E;
			particles[i].position.x = particles[i].position.x < bounds.x ? bounds.x : bounds.w - 1.f;
		}

		if (particles[i].position.y < bounds.y || particles[i].position.y > bounds.h)
		{
			particles[i].velocity.y = -particles[i].velocity.y * WALL_COLLISION_E;
			particles[i].position.y = particles[i].position.y < bounds.y ? bounds.y : bounds.h - 1.f;
		}
	}
}

EXPORT INIT_GAME(initGame);
EXPORT UPDATE_GAME(updateGame);
EXPORT DRAW_GAME(drawGame);

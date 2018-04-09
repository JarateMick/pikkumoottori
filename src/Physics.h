#pragma once 
#include "vec2.h" // visual studio drunk
#define MAX_P_BODIES 100

typedef struct 
{
	Vec2 pos[MAX_P_BODIES];
	Vec2 vel[MAX_P_BODIES];
	Vec2 acc[MAX_P_BODIES];
	Vec2 size[MAX_P_BODIES];
	float m[MAX_P_BODIES];

	float rot[MAX_P_BODIES];
	float torque[MAX_P_BODIES];
	float angularVel[MAX_P_BODIES];

	float momentOfInertia[MAX_P_BODIES];

	Vec2 verticesPositions[MAX_P_BODIES * 4];

	int count;
} PhysicsBodies;

typedef struct 
{
	int selectedBody;

	bool32 dragging;
	Vec2 mouseMoveVector[10];
	Vec2 base;
	int current;
} PhysicsController;


// typedef struct GameState GameState;
static void testPhysicsBodies(GameState* state, GraphicsContext* c, int count);

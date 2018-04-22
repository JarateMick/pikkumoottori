#pragma once 
#include "vec2.h" // visual studio drunk
#define MAX_P_BODIES 100
#define PHYSICS_STEPS 12

typedef struct {
	int first;
	int second;

	float length;
	float k;

	int vertIndexA;
	int vertIndexB;
} SpringPair;

typedef struct
{
	SpringPair springPairs[MAX_P_BODIES / 2];
	int springCount;
} Springs;

#define MAKE_SPRING(a, b, len, k, vertA, vertB) { a, b, len, k, vertA, vertB }

static SpringPair inline springpair(int a, int b, float length, float k, int vertA, int vertB)
{
	SpringPair result = MAKE_SPRING(a, b, length, k, vertA, vertB);
	return result;
}

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

	Springs springs;

	int frozenBodies[MAX_P_BODIES];
	int frozenCount;
} PhysicsBodies;
// todo: benchmark and bundle relevant

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
static void SetPhysicsObject(PhysicsBodies* b, int id, vec2 pos, vec2 size, vec2 vel, float rot, float angularVel);
static void recalculateMomentumOfInertia(PhysicsBodies* b);
static void InitPhysicsTest(PhysicsBodies* b);

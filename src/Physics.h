#pragma once 

#define MAX_P_BODIES 100
typedef struct 
{
	Vec2 pos[MAX_P_BODIES];
	Vec2 vel[MAX_P_BODIES];
	Vec2 acc[MAX_P_BODIES];
	Vec2 size[MAX_P_BODIES];
	float m[MAX_P_BODIES];
	float rot[MAX_P_BODIES];

	vec2 verticesPositions[MAX_P_BODIES * 4];

	int count;
} PhysicsBodies;


// typedef struct GameState GameState;
static void testPhysicsBodies(GameState* state, GraphicsContext* c, int count);

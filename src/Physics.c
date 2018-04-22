#include "application/game.h" // visual studio drunk
#include "Physics.h"
#include <float.h>

// uusi K * V(jousen muutos nopeus) = F
// liikettä vastaan
static void addSpring(PhysicsBodies* bodies, int a, int b, float len, float k,
	int vertA, int vertB)
{
	Springs* springs = &bodies->springs;
	int slot = springs->springCount;

	springs->springPairs[slot] = springpair(a, b, len, k, vertA, vertB);
	springs->springCount += 1;
}

static void updateSprings(PhysicsBodies* bodies)
{
	Springs* s = &bodies->springs;

	for (int i = 0; i < s->springCount; ++i)
	{
		SpringPair* p = s->springPairs + i;

		int bodyA = p->first;
		int bodyB = p->second;

		// Vec2 verticesPositions[MAX_P_BODIES * 4];
		vec2 distVec = vec2_subv(bodies->verticesPositions + (bodyA * 4 + p->vertIndexA),
			bodies->verticesPositions + (bodyB * 4 + p->vertIndexB));
		float distance = vec2_len(&distVec);

		vec2_normalizeInPlace(&distVec);

		float deltalength = p->length - distance;
		float force = deltalength * p->k;

		vec2_mul_s(&distVec, &distVec, force);

		vec2_add_v(bodies->vel + bodyA, bodies->vel + bodyA, &distVec);
		vec2_sub_v(bodies->vel + bodyB, bodies->vel + bodyB, &distVec);
	}
}

static inline void freezebody(PhysicsBodies* bodies, int id)
{
	bodies->frozenBodies[bodies->frozenCount++] = id;
}
static inline void unfreezebody(PhysicsBodies* bodies, int id)
{
	int lastID = bodies->frozenBodies[bodies->frozenCount];
	bodies->frozenBodies[id] = lastID;
	bodies->frozenCount--;
}

static void testPhysicsBodies(GameState* state, GraphicsContext* c, int count)
{
	// init stuff
	PhysicsBodies* bs = state->bodies;
	count = 6;

	memset(bs, 0, sizeof(PhysicsBodies));

	for (int i = 0; i < count; ++i)
	{
		bs->pos[i] = V2(i * 50.f, 150.f);
		bs->size[i] = V2(60.f, 60.f);
	}

	bs->count = count;

	ResourceHolder* h = &c->resourceHolder;
	Texture2D* tex = getTexture(h, Texture_box);
	for (int i = 0; i < MAX_P_BODIES; ++i)
	{
		c->sprites.ids[i] = tex->ID;
		bs->m[i] = 5.f;
	}



	InitPhysicsTest(bs);





	memset(&state->physicsControls, 0, sizeof(PhysicsController));


	// ja niin hän sanoi tulkoon mies
	const float manX = 6000.f;
	const float manY = 2000.f;

	// keho
	SetPhysicsObject(bs, 10, V2(manX, manY), V2(40.f, 60.f), V2(0.f, 0.f),
		0.f, 0.f);

	// pää
	SetPhysicsObject(bs, 11, V2(manX, manY + 25.f), V2(20.f, 20.f), V2(0.f, 0.f),
		0.f, 0.f);

	SetPhysicsObject(bs, 8, V2(manX + 20.f, manY - 20.f), V2(20.f, 40.f), V2(0.f, 0.f),
		0.f, 0.f);
	SetPhysicsObject(bs, 9, V2(manX - 20.f, manY - 20.f), V2(20.f, 40.f), V2(0.f, 0.f),
		0.f, 0.f);
	bs->count = 12;

#if 0
	const float springLength = 30.f;
	const float springConstrain = 0.01f;
	addSpring(bs, 10, 6, springLength, springConstrain, 0, 3); // v jalka 
	addSpring(bs, 10, 6, springLength, springConstrain, 0, 1);

	addSpring(bs, 10, 7, springLength, springConstrain, 1, 2); // o jalka
	addSpring(bs, 10, 7, springLength, springConstrain, 1, 0);

	addSpring(bs, 10, 8, springLength, springConstrain, 2, 3); // v käsi
	addSpring(bs, 10, 8, springLength, springConstrain, 2, 1);

	addSpring(bs, 10, 9, springLength, springConstrain, 3, 2);
	addSpring(bs, 10, 9, springLength, springConstrain, 3, 0); // o käsi

	addSpring(bs, 10, 11, springLength, springConstrain, 0, 0);
	addSpring(bs, 10, 11, springLength, springConstrain, 2, 1);
#endif
	// addSpring(bs, 0, 1, 40.f, 0.1f, 0, 1);
	// addSpring(bs, 1, 2, 30.f, 30.f);
	recalculateMomentumOfInertia(bs);

	c->sprites.positions = bs->pos;
	c->sprites.sizes = bs->size;
	c->sprites.rotation = bs->rot;
	c->sprites.count = bs->count;

	for (int i = 0; i < bs->count; ++i)
	{
		c->sprites.colors[i] = V4(rand(), rand(), rand(), 1.f);
	}

	for (int i = 5; i < bs->count; ++i)
	{
		float co = rand();
		c->sprites.colors[i] = V4(co, co, co, 1.f);
	}
	c->sprites.colors[rand() % bs->count] = V4(1.f, 0.f, 0.f, 1.f);
	c->sprites.colors[rand() % bs->count] = V4(1.f, 0.f, 1.f, 1.f);
}

static inline Vec2 rotatePoint(Vec2* point, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	return V2(point->x * c + point->y * s, point->y * c - point->x * s);
}

static inline void setSprites(Sprites* s, Vec2 pos, Vec2 size, int texId, int id)
{
	s->positions[id] = pos;
	s->rotation[id] = 0.f;
	s->sizes[id] = size;
	s->ids[id] = texId;
}

static inline void setSpritesRo(Sprites* s, Vec2 pos, Vec2 size, int texId, int id, float rot)
{
	s->positions[id] = pos;
	s->rotation[id] = rot;
	s->sizes[id] = size;
	s->ids[id] = texId;
}

static int pointInsideTringle(Vec2* p1, Vec2* p2, Vec2* p3, Vec2* point)
{
	vec2 v0 = vec2_subv(p2, p1);
	vec2 v1 = vec2_subv(p3, p1);
	vec2 v2 = vec2_subv(point, p1);

	float dot00 = dotProduct(&v0, &v0);
	float dot01 = dotProduct(&v0, &v1);
	float dot02 = dotProduct(&v0, &v2);
	float dot11 = dotProduct(&v1, &v1);
	float dot12 = dotProduct(&v1, &v2);

	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0) && (v >= 0) && (u + v < 1);
}

static bool32 pointInsideRectangle(Vec2* a, Vec2* b, Vec2* c, Vec2* d, Vec2* point)
{
	vec2 am = vec2_subv(a, point);
	vec2 ab = vec2_subv(a, b);
	vec2 ad = vec2_subv(a, d);

	float dot0 = dotProduct(&am, &ab);
	float dot1 = dotProduct(&ab, &ab);
	float dot2 = dotProduct(&am, &ad);
	float dot3 = dotProduct(&ad, &ad);

	return ((0 < dot0 && dot0 < dot1) && (0 < dot2 && dot2 < dot3));
}

#define GRAV -9.81f * 60.f


vec2 drawThisVec;
vec2 from;

static void SetPhysicsObject(PhysicsBodies* b, int id, vec2 pos, vec2 size, vec2 vel, float rot, float angularVel)
{
	b->pos[id] = pos;
	b->vel[id] = V2(0.f, 0.f);
	b->size[id] = size;
	b->acc[id] = vel;
	b->angularVel[id] = angularVel;
	b->rot[id] = rot;

	if (b->m[id] == 0.f)
		b->m[id] = 5.f;
}

static void recalculateMomentumOfInertia(PhysicsBodies* b)
{
	for (int i = 0; i < b->count; ++i)
	{
		float momentumOfInertia = (1.f / 12.f) * b->m[i] * (pow2(b->size[i].x) + pow2(b->size[i].y));
		b->momentOfInertia[i] = momentumOfInertia;
		printf("%f \n", momentumOfInertia);
	}
}

void InitPhysicsTest(PhysicsBodies* b)
{
	SetPhysicsObject(b, 0, V2(100.f, 200.f), V2(40.f, 40.f), V2(0.f, 0.f), 30.f, 0.0f);
	SetPhysicsObject(b, 1, V2(100.f, 300.f), V2(40.f, 40.f), V2(0.f, -50.f), 2.14f, 0.1f);
	SetPhysicsObject(b, 2, V2(40.f, 280.f), V2(40.f, 60.f), V2(0.f, 0.0f), 0.f, 0.0f);
	SetPhysicsObject(b, 3, V2(37.f, 140.f), V2(40.f, 60.f), V2(0.f, 0.0f), 3.14f, 0.0f);
	SetPhysicsObject(b, 5, V2(-50.f, 666.f), V2(60.f, 40.f), V2(0.f, 0.0f), 0.0f, 1.f); // hasdf

	SetPhysicsObject(b, 8, V2(-150.f, 300.f), V2(60.f, 40.f), V2(0.f, 0.0f), 0.0f, 2.f); // hasdf
	SetPhysicsObject(b, 9, V2(-100.f, 400.f), V2(60.f, 40.f), V2(0.f, 0.0f), 0.0f, 4.f); // hasdf

	// FLOORS
	SetPhysicsObject(b, 4, V2(50.f, 50.f), V2(350.f, 40.f), V2(0.f, 0.0f), 0.f, 0.0f);
	SetPhysicsObject(b, 6, V2(-240.0f, 210.f), V2(350.f, 40.f), V2(0.f, 0.f), -45.f, 0.f);
	SetPhysicsObject(b, 7, V2(366, 177.f), V2(350.f, 40.f), V2(0.f, 0.f), 45.f, 0.f);

	freezebody(b, 4); 
	freezebody(b, 6); 
	freezebody(b, 7); 

	b->m[4] = 1000.f;
	b->m[6] = 1000.f;
	b->m[7] = 1000.f;


	recalculateMomentumOfInertia(b);
}

static void InitPhysicsTest2(PhysicsBodies* b)
{
	SetPhysicsObject(b, 0, V2(410.f, 100.f), V2(40.f, 40.f), V2(0.f, 0.f), 0.f, 0.4f);
	SetPhysicsObject(b, 1, V2(460.f, 115.f), V2(40.f, 40.f), V2(0.f, 0.f), 1.f, 0.0f);
	recalculateMomentumOfInertia(b);
}

static int32 physics_checkCollision(PhysicsBodies* bs, int iIndex, int jIndex)
{
	for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) // joka vertex
	{
		bool32 collides = pointInsideRectangle(bs->verticesPositions + iIndex + 0,
			bs->verticesPositions + iIndex + 1,
			bs->verticesPositions + iIndex + 3,
			bs->verticesPositions + iIndex + 2,
			bs->verticesPositions + jIndex + vertexIndex);

		if (collides)
		{
			// coll = vertexIndex + 4;
			return vertexIndex;
		}
	}
	return 0;
}

struct {
	Vec2 collisionPoint;
	Vec2 collisionNormal;

	Vec2 start;
	Vec2 end;
} debugInfo;

static void physics_handleCollision_2(PhysicsBodies* bs, int collIndex, int bodyA, int bodyB, Vec2* normalIn)
{
	vec2* collisionPoint = bs->verticesPositions + collIndex;

	vec2 normal = *normalIn;
	vec2 dist = normal;
	vec2_normalizeInPlace(&dist);

	vec2_normalizeInPlace(&normal);

#ifndef NO_DEBUG
	debugInfo.collisionPoint = *collisionPoint;
	drawThisVec = dist;
	from = *collisionPoint;
	debugInfo.collisionNormal = dist;
#endif

	{
		float j;
		float e = 1.0f;

		float upper = (-(1 + e));

		Vec2 contactAP = vec2_subv(collisionPoint, bodyA + bs->pos);
		Vec2 contactBP = vec2_subv(collisionPoint, bodyB + bs->pos);

		vec2 velA = bs->vel[bodyA];
		vec2 velB = bs->vel[bodyB];
		float angularA = bs->angularVel[bodyA];
		float angularB = bs->angularVel[bodyB];

		contactAP = turn(&contactAP);
		contactBP = turn(&contactBP);

		vec2 ang = vec2_mul(&contactAP, angularA);
		vec2 bng = vec2_mul(&contactBP, angularB);

		vec2_add_v(&velA, &velA, &ang);
		vec2_add_v(&velB, &velB, &bng);

		Vec2 vab = vec2_subv(&velA, &velB);

		float velAlongNormal = dotProduct(&vab, &dist);

		if (velAlongNormal > 0)
			return;

		float dot = dotProduct(&vab, &normal);
		upper = upper * dot;

		float nn = dotProduct(&normal, &normal);

		float massOverOneA = 1.f / bs->m[bodyA];
		float massOverOneB = 1.f / bs->m[bodyB];

		float totalMass = massOverOneA + massOverOneB;

		float contactAPN = dotProduct(&contactAP, &normal);
		float contactBPN = dotProduct(&contactBP, &normal);
		float contactAPN2 = (contactAPN*contactAPN) / bs->momentOfInertia[bodyA];
		float contactBPN2 = (contactBPN*contactBPN) / bs->momentOfInertia[bodyB];

		float lower = /*nn*/ 1.f * totalMass + contactAPN2 + contactBPN2;
		j = upper / lower;

		float a = j / bs->m[bodyA];
		Vec2 v2a;
		vec2_mul_s(&v2a, &normal, a);

		float b = -j / bs->m[bodyB];
		Vec2 v2b;
		vec2_mul_s(&v2b, &normal, b);


		vec2_add_v(bs->acc + bodyA, bs->acc + bodyA, &v2a);
		vec2_add_v(bs->acc + bodyB, bs->acc + bodyB, &v2b);
		Vec2 jn = vec2_mul(&normal, j);

		float aAngular = dotProduct(&contactAP, &jn);
		float bAngular = dotProduct(&contactBP, &jn);
		aAngular /= bs->momentOfInertia[bodyA];
		bAngular /= bs->momentOfInertia[bodyB];

		bs->angularVel[bodyA] += aAngular;
		bs->angularVel[bodyB] -= bAngular;
	}
}

static float IntervalDistance(float minA, float maxA, float minB, float maxB) {
	if (minA < minB) {
		return minB - maxA;
	}
	else {
		return minA - maxB;
	}
}

static bool32 polygonIntersection(PhysicsBodies* bodies, int bodyA, int bodyB, Vec2* outNormal, int* outId)
{
	// printf("body a: rot %f \n", *(bodies->rot + bodyA));
	// printf("body b: rot %f \n", *(bodies->rot + bodyB));

	float d = FLT_MAX;
	int bestI = -1;

	int jj = 0;
	int ii = 0;

	Vec2* centerA = bodies->pos + bodyA;
	Vec2* centerB = bodies->pos + bodyB;

	Vec2 bestNormal = { 0.f, 0.f };

	int collBody;
	int aIndex;
	int bIndex;

	for (int i = 0; i < 2; ++i)
	{
		int bodyId = (i == 0) ? bodyA : bodyB;

		for (int i1 = 0; i1 < 4; ++i1)
		{
			int i2 = (i1 + 1) % 4;

			Vec2* point1 = bodies->verticesPositions + (bodyId * 4) + i1;
			Vec2* point2 = bodies->verticesPositions + (bodyId * 4) + i2;

			// Vec2 normal = vec2_subv(point1, point2);
			Vec2 normal = V2(point2->y - point1->y, point1->x - point2->x);

			float minA = FLT_MAX,
				maxA = -FLT_MIN;

			for (int j = 0; j < 4; ++j) // joka A:n pisteelle
			{
				Vec2* p = bodies->verticesPositions + (bodyA * 4) + j;
				float projected = normal.x * p->x + normal.y * p->y;

				if (projected < minA)
				{
					minA = projected;
					aIndex = j;
				}

				if (projected > maxA)
				{
					maxA = projected;
				}
			}

			float minB = FLT_MAX,
				maxB = -FLT_MAX;

			for (int j = 0; j < 4; ++j) // joka B:n pisteelle
			{
				Vec2* p = bodies->verticesPositions + (bodyB * 4) + j;
				float projected = normal.x * p->x + normal.y * p->y;

				if (projected < minB)
				{
					minB = projected;
				}

				if (projected > maxB)
				{
					maxB = projected;
				}
			}

			if (maxA < minB || maxB < minA)
				return false;

			// s - v
			// float distance = p->x * centerB->x + p->y * centerB->y;
			float interval = IntervalDistance(minA, maxA, minB, maxB);
			interval = abs(interval);
			if (interval < d)
			{
				bestI = i1;
				d = interval;
				bestNormal = normal;
				collBody = bodyId;
			}
		}
	}

	// dots
	Vec2 aToB = vec2_subv(centerB, centerA);
	vec2_normalizeInPlace(&aToB);
	float dotAB = bestNormal.x * aToB.x + bestNormal.y * aToB.y;

	if (dotAB < 0.f) {
		// k''n'n tai en
		bestNormal.x = -bestNormal.x;
		bestNormal.y = -bestNormal.y;
	}
	*outNormal = bestNormal;
	*outId = collBody;

#if 0
	printf("%i \n", bestI);
	printf(" is sameh dir: %i ", dotAB > 0.f);

	debugInfo.collisionNormal = bestNormal;
	debugInfo.collisionPoint = V2(50.f, 50.f);

	debugInfo.start = *centerA;
	debugInfo.end = *centerB;

	printf("%f %f\n", centerA->x, centerA->y);
	printf("%f %f\n", centerB->x, centerB->y);
#endif

	return true;
}

///////////////////////////////////////////////////////////////
struct {
	Vec2 start;
	Vec2 end;
} debug_lines[10000];

int d_count;

void pushLine(float x, float y, float x1, float y1) {
	// ASSERT
	debug_lines[d_count].start = V2(x, y);
	debug_lines[d_count++].end = V2(x1, y1);
}

void clear() {
	d_count = 0;
}
///////////////////////////////////////////////////////////////


static inline float toRadians(float angle)
{
	return angle * M_PI / 180.f;
}

static inline bool32 physics_isSelected(PhysicsController* c)
{
	const int notSelected = -1;
	return c->selectedBody != notSelected;
}

static inline void physics_rotateSelected(GameState* state, float rotation)
{
	PhysicsController* c = &state->physicsControls;
	PhysicsBodies* bodies = state->bodies;

	if (physics_isSelected(c))
	{
		int id = c->selectedBody;
		bodies->rot[id] += rotation;
	}
}

static void physics_onControl(EngineContext* c, GameState* state, float dt)
{
	int mouseWheel = c->controller.mouseWheel;

	float rotSpeed = 35.f * dt;
	if (mouseWheel == -1)
	{
		physics_rotateSelected(state, rotSpeed);
	}
	else if (mouseWheel == 1)
	{
		physics_rotateSelected(state, -rotSpeed);
	}

	PhysicsBodies* bodies = state->bodies;
	if (isMousePressed(c, 0))
	{
		for (int i = 0; i < bodies->count; ++i)
		{
			vec2* mouse = &c->controller.mouseWorldPos;
			vec2* vpos = bodies->verticesPositions + (i * 4);
			if (pointInsideRectangle(vpos + 0, vpos + 1, vpos + 2, vpos + 3, mouse))
			{
				state->physicsControls.selectedBody = i;
				state->physicsControls.dragging = true;
				state->physicsControls.base = *mouse;
				memset(state->physicsControls.mouseMoveVector, 0, sizeof(state->physicsControls.mouseMoveVector));
			}
		}
	}


	// TODO: calc mouse movement in controller
	PhysicsController* controller = &state->physicsControls;
	if (controller->dragging)
	{
		bodies->pos[controller->selectedBody] = c->controller.mouseWorldPos;

		if (!c->controller.mouseDown)
		{
			vec2 mouseDiff = { 0.f, 0.f };
			for (int i = 0; i < 5; ++i)
			{
				vec2_normalizeInPlace(controller->mouseMoveVector + i);
				vec2_add_v(&mouseDiff, &mouseDiff, controller->mouseMoveVector + i);
			}

			vec2_normalizeInPlace(&mouseDiff);
			vec2_mul(&mouseDiff, 1000.f);
			bodies->vel[controller->selectedBody] = vec2_mul(&mouseDiff, 0.1f);

			controller->dragging = false;
			controller->selectedBody = -1;

		}
		else
		{
			if (controller->current == 5)
			{
				controller->current = 0;
			}
			vec2* mouse = &c->controller.mouseWorldPos;

			vec2 submouse = vec2_subv(mouse, &controller->base);
			controller->mouseMoveVector[controller->current] = submouse;

			(controller->current)++;
			controller->base = *mouse;
		}
	}
}


static void updateBodies(EngineContext* core, GameState* state, GraphicsContext* c, Vec2 mouse, float dt)
{
	PhysicsBodies* bs = state->bodies;
	int count = bs->count;

	memset(bs->acc, 0, sizeof(Vec2) * MAX_P_BODIES);


	physics_onControl(core, state, dt);
	// Compute Forces
	// gravity

#if 1
	for (int i = 0; i < bs->count; ++i) {
		float force = GRAV / bs->m[i];
		bs->acc[i].y += force * dt;

		//if (bs->pos[i].y < -50505050238250.f)
		//{
		//	bs->acc[i].y = 0.f;
		//	bs->vel[i].y = 0.f;
		//}
	}
#endif


	{ // Controls
		const float spd = 0.8f;
		if (isKeyDown(core, Key_R))
		{
			bs->rot[1] += spd * dt;
		}
		if (isKeyDown(core, Key_T))
		{
			bs->rot[1] -= spd * dt;
		}

		// bs->rot[0] += 0.001f; // aka grad

		static bool32 moveToMouse = 0;

		if (isKeyDown(core, Key_G))
		{
			moveToMouse = !moveToMouse;
		}

		if (moveToMouse)
		{
			bs->pos[1] = mouse;
			bs->vel[1] = V2(0.f, 0.f);
			bs->acc[1] = V2(0.f, 0.f);
		}

		if (isKeyPressed(core, Key_F))
		{
			InitPhysicsTest(bs);
		}

		if (isKeyPressed(core, Key_V))
		{
			InitPhysicsTest2(bs);
		}
	}

#if 1      // calculate vertice positions
	ResourceHolder* h = &c->resourceHolder;
	Texture2D* tex = getTexture(h, Texture_box);

	vec4 green = { 0.f, 1.f, 0.f, 1.f };
	vec4 red = { 1.f, 0.f, 0.f, 1.f };

	for (int i = 0; i < count; ++i)
	{

		Vec2* pos = &bs->pos[i];
		Vec2* siz = &bs->size[i];
		Vec2 half = vec2_mul(siz, 0.5f);

		float w = half.x;
		float h = half.y;

		Vec2 bl = V2(-w, -h);
		Vec2 br = V2(w, -h);
		Vec2 tl = V2(-w, h);
		Vec2 tr = V2(w, h);

		float angle = bs->rot[i];
		Vec2 p1 = rotatePoint(&bl, -angle);
		Vec2 p2 = rotatePoint(&br, -angle);
		Vec2 p3 = rotatePoint(&tl, -angle);
		Vec2 p4 = rotatePoint(&tr, -angle);


		vec2_add_v(&p1, &p1, pos);
		vec2_add_v(&p2, &p2, pos);
		vec2_add_v(&p3, &p3, pos);
		vec2_add_v(&p4, &p4, pos);

		bs->verticesPositions[i * 4 + 0] = p1;
		bs->verticesPositions[i * 4 + 1] = p2;
		bs->verticesPositions[i * 4 + 2] = p4;     // BIG SORRY
		bs->verticesPositions[i * 4 + 3] = p3;
	}
#endif

	int  outId;
	Vec2 outNormal;
	for (int ii = 0; ii < count; ++ii)
	{
		for (int jj = ii + 1; jj < count; ++jj)
		{
			if (polygonIntersection(bs, ii, jj, &outNormal, &outId))
			{
				int i = outId == ii ? ii : jj;
				int j = outId == ii ? jj : ii;

				// Hahahahahhahahahhahhaaahahahhahah
				Vec2* vp = bs->verticesPositions;

				int other = outId == ii ? jj : ii;
				int collIndex = (other * 4) + 0;

				if (outId == jj) // normal 180* v''rin
				{
					outNormal.x = -outNormal.x;
					outNormal.y = -outNormal.y;
				}

				for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) // joka vertex
				{
					int collides = pointInsideRectangle(vp + (outId * 4) + 0, vp + (outId * 4) + 1,
						vp + (outId * 4) + 2,
						vp + (outId * 4) + 3, vp + (other * 4) + vertexIndex);

					if (collides)
					{
						// printf("[%i]\n", vertexIndex);
						collIndex = (other * 4) + vertexIndex;
						break;
					}
				}
				Vec2* d_collision = collIndex + vp;
				pushLine(d_collision->x, d_collision->y, d_collision->x + 1.f, d_collision->y + 1.f);

				// printf("out [%i %i]\n", other, outId);
				// physics_handleCollision_2(bs, collIndex, jj, ii, &outNormal);
				physics_handleCollision_2(bs, collIndex, other, outId, &outNormal);
			}
		}
	}

#if 0
	for (int i = 0; i < ArrayCount(debug_collides); ++i) {
		bool32 coll = debug_collides[i];
		c->sprites.colors[i] = coll ? green : red;
		}
#endif


	for (int i = 0; i < bs->frozenCount; ++i)
	{
		int entId = bs->frozenBodies[i];

		// bs->angularVel[entId] = 0.f;
		// bs->rot[entId] = 0.f;
		bs->acc[entId] = V2(0.f, 0.f);
		bs->vel[entId] = V2(0.f, 0.f);
	}

	vec2_add_vs(bs->vel, bs->vel, bs->acc, bs->count);

	updateSprings(bs);

	for (int i = 0; i < count; ++i)
	{
		vec2* pos = bs->pos + i;
		vec2* vel = bs->vel + i;

		vec2 veldt;
		vec2_mul_s(&veldt, vel, dt);
		vec2_add_v(pos, pos, &veldt);

		if (pos->x < -500.f || pos->x > 2000.f)
			vel->x = -vel->x;

		if (pos->y < -100000000000.f || pos->y > 10000.f)
			vel->y = -vel->y;
	}

	// angularVelocity += torque * (1.0f / momentOfInertia) * dt
	// position += velocity * dt
	// orient += angularVelocity * dt

	for (int i = 0; i < count; ++i)
	{
		const float momentOfInertia = 1.f;
		bs->angularVel[i] += bs->torque[i] * (1.0f / momentOfInertia) * dt;
		bs->rot[i] += (bs->angularVel[i] * dt);
	}
	}

static void drawBodies(EngineContext* context, GraphicsContext* c, GameState* state, int count)
{
	// draw some force arrows maybe

	vec2 unit = vec2_mul(&drawThisVec, 10.f);
	vec2 destination = vec2_addv(&from, &unit);
	// drawLine(c, &from, &destination, 0xFFFFFFFF);

	unit = vec2_mul(&debugInfo.collisionNormal, 10.f);
	destination = vec2_addv(&debugInfo.collisionPoint, &unit);
	// drawLine(c, &debugInfo.collisionPoint, &destination, 0xFFFFFFFF);
	// drawLine(c, &debugInfo.start, &debugInfo.end, 0xFFFFFFFF);


	for (int i = 0; i < d_count; ++i)
	{
		drawLine(c, &debug_lines[i].start, &debug_lines[i].end, 0xFFFFFFFF);
	}
	d_count = 0;


	PhysicsBodies* bodies = state->bodies;

	// reduclant
	Texture2D* box = getTexture(&context->context.resourceHolder, Texture_box);
	int STARTID = 0;
	for (int i = 0; i < bodies->count; ++i)
	{
		setSpritesRo(&c->sprites, bodies->pos[i], bodies->size[i], box->ID, i, bodies->rot[i]);
		STARTID = i + 1;
	}

	// draw some googly eyes
	Texture2D* eye = getTexture(&context->context.resourceHolder, Texture_circle);
	Vec2 MousePos = context->controller.mouseWorldPos;
	for (int i = 0; i < bodies->count; ++i)
	{
		Vec2 p0 = bodies->verticesPositions[i * 4 + 0];
		Vec2 p1 = bodies->verticesPositions[i * 4 + 1];
		Vec2 p2 = bodies->verticesPositions[i * 4 + 2];
		Vec2 p3 = bodies->verticesPositions[i * 4 + 3];

		Vec2 cross0 = vec2_subv(&p0, &p2);
		Vec2 cross1 = vec2_subv(&p1, &p3);
		const float eyeCornerOffset = -0.3f;

		float l0 = vec2_len(&cross0);
		float l1 = vec2_len(&cross1);

		vec2_normalizeInPlace(&cross0);
		vec2_normalizeInPlace(&cross1);

		Vec2 o0 = vec2_mul(&cross0, l0 * eyeCornerOffset);
		Vec2 o1 = vec2_mul(&cross1, l1 * eyeCornerOffset);

		Vec2 eyePosition0 = vec2_addv(&o0, &p0);
		Vec2 eyePosition1 = vec2_addv(&o1, &p1);


		setSprites(&c->sprites, eyePosition0, V2(10.f, 10.f), eye->ID, i * 4 + STARTID + 0);
		setSprites(&c->sprites, eyePosition1, V2(10.f, 10.f), eye->ID, i * 4 + STARTID + 1);
		c->sprites.colors[i * 4 + STARTID + 0] = V4(1.f, 1.f, 1.f, 1.f);
		c->sprites.colors[i * 4 + STARTID + 1] = V4(1.f, 1.f, 1.f, 1.f);


		const float innerEyeLen = 4.f;

		Vec2 toMouse0 = vec2_subv(&MousePos, &eyePosition0);
		Vec2 toMouse1 = vec2_subv(&MousePos, &eyePosition1);

		vec2_normalizeInPlace(&toMouse0);
		vec2_normalizeInPlace(&toMouse1);

		Vec2 inEye0 = vec2_mul(&toMouse0, innerEyeLen);
		Vec2 inEye1 = vec2_mul(&toMouse1, innerEyeLen);

		vec2_add_v(&inEye0, &inEye0, &eyePosition0);
		vec2_add_v(&inEye1, &inEye1, &eyePosition1);

		setSprites(&c->sprites, inEye0, V2(3.f, 3.f), eye->ID, i * 4 + STARTID + 2);
		setSprites(&c->sprites, inEye1, V2(3.f, 3.f), eye->ID, i * 4 + STARTID + 3);
		c->sprites.colors[i * 4 + STARTID + 2] = V4(0.f, 0.f, 0.f, 1.f);
		c->sprites.colors[i * 4 + STARTID + 3] = V4(0.f, 0.f, 0.f, 1.f);

		// setSprites(&c->sprites, , V2(4.f, 4.f), eye->ID, i + 13);
		c->sprites.count = i * 4 + STARTID;
	}
}

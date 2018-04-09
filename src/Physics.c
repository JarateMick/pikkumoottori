#include "application\game.h" // visual studio drunk
#include "Physics.h"
#include <float.h>

static float dotProduct(Vec2* a, Vec2* b)
{
	return a->x * b->x + a->y * b->y;
}

static float crossProduct(Vec2* a, Vec2* b)
{
	return a->x * b->y - a->y * b->x;
}

static inline float pow2(float x)
{
	return x * x;
}

static void testPhysicsBodies(GameState* state, GraphicsContext* c, int count)
{
	// init stuff
	PhysicsBodies* bs = state->bodies;

	count = 6;

	for (int i = 0; i < count; ++i)
	{
		bs->pos[i] = V2(i * 25.f, 150.f);
		bs->size[i] = V2(20.f, 20.f);
		bs->m[i] = 1.f;
	}

	memset(bs->acc, 0, sizeof(Vec2) * MAX_P_BODIES);
	memset(bs->vel, 0, sizeof(Vec2) * MAX_P_BODIES);
	memset(bs->rot, 0, sizeof(float) * MAX_P_BODIES);
	memset(bs->angularVel, 0, sizeof(float) * MAX_P_BODIES);
	memset(bs->torque, 0, sizeof(float) * MAX_P_BODIES);

	bs->count = count;


	// Drawing quads
	ResourceHolder* h = &c->resourceHolder;
	Texture2D* tex = getTexture(h, Texture_box);

	c->sprites.positions = bs->pos;
	c->sprites.sizes = bs->size;
	c->sprites.rotation = bs->rot;
	c->sprites.count = count;

	for (int i = 0; i < count + 4; ++i)
	{
		c->sprites.ids[i] = tex->ID;
	}

	bs->pos[4] = V2(100.f, 60.f);
	bs->size[4] = V2(400.f, 20.f);

	memset(&state->physicsControls, 0, sizeof(PhysicsController));


	vec2 test[] = { { 0.f, 0.f }, { 0.f, 20.f }, { 20.f, 0.f }, { 20.f, 20.f } };
	float sum = 0.f;
	for (int i = 0; i < 4; ++i) {
		vec2 dist = vec2_subv(test + 0, test + 1);
		sum += (dist.x * dist.x + dist.y * dist.y) * 0.25f;
		// (dotProduct(test + i, test + 0) * 0.25f);
	}


	// float momentumOfInertia = (1.f / 12.f) * 1.f * (20.f * 20.f + 20.f * 20.f);

	for (int i = 0; i < count; ++i)
	{
		float momentumOfInertia = (1.f / 12.f) * bs->m[i] * (pow2(bs->size[i].x) + pow2(bs->size[i].y));
		bs->momentOfInertia[i] = momentumOfInertia;
		printf("%f \n", momentumOfInertia);
	}

	printf("%f \n", sum);
}

static inline Vec2 rotatePoint(Vec2* point, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	return V2(point->x * c + point->y * s, point->y * c - point->x * s);
}

static inline setSprites(Sprites* s, Vec2 pos, Vec2 size, int texId, int id)
{
	s->positions[id] = pos;
	s->rotation[id] = 0.f;
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

static void applyImpulse(PhysicsBodies* b, Vec2* contact, Vec2 impulse, int id)
{
	float invMass = 1.f / b->m[id];
	vec2_mul_s(&impulse, &impulse, invMass);
	vec2_add_v(b->vel + id, b->vel + id, &impulse);

	// angularVelocity += 1.0f / inertia * Cross( contactVector, impulse );
}

#define GRAV -9.81f / 50.f

vec2 drawThisVec;
vec2 from;

static void InitPhysicsTest(PhysicsBodies* b)
{
	b->pos[0] = V2(100.f, 250.f);
	b->vel[0] = V2(0.f, 0.f);
	b->acc[0] = V2(0.f, 0.f);
	b->rot[0] = 2.f;
	b->angularVel[1] = 0.f;
	b->size[1] = V2(40.f, 20.f);

	b->pos[1] = V2(97.f, 300.f);
	b->vel[1] = V2(0.f, 0.f);
	b->acc[1] = V2(0.f, -0.1f);
	b->rot[1] = 1.f;
	b->angularVel[0] = 0.f;

	b->pos[3] = V2(40.f, 250.f);
	b->vel[3] = V2(0.f, 0.f);
	b->acc[3] = V2(0.f, 0.f);
	b->rot[3] = 2.f;
	b->angularVel[3] = 0.f;

	b->pos[2] = V2(37.f, 300.f);
	b->size[2] = V2(40.f, 20.f);
	b->vel[2] = V2(0.f, 0.f);
	b->acc[2] = V2(0.f, 0.0f);
	b->rot[2] = 1.f;
	b->angularVel[2] = 0.f;


	b->vel[5] = V2(0.f, 0.f);
	b->pos[5] = V2(40.f, 200.f);
	b->size[5] = V2(40.f, 20.f);
	b->rot[5] = 0.f;
	b->angularVel[5] = 0.f;

	b->pos[4] = V2(400.f, 20.f);
	b->vel[4] = V2(0.f, 0.f);
	b->acc[4] = V2(0.f, -0.1f);
	b->angularVel[4] = 0.f;
	b->rot[4] = 0.f;

	for (int i = 0; i < 5; ++i)
	{
		float momentumOfInertia = (1.f / 12.f) * b->m[i] * (pow2(b->size[i].x) + pow2(b->size[i].y));
		b->momentOfInertia[i] = momentumOfInertia;
		printf("%f \n", momentumOfInertia);
	}
}

static void SetPhysicsObject(PhysicsBodies* b, int id, vec2 pos, vec2 size, vec2 vel, float rot, float angularVel)
{
	b->pos[id] = pos;
	b->vel[id] = V2(0.f, 0.f);
	b->size[id] = size;
	b->acc[id] = vel;
	b->angularVel[id] = angularVel;
	b->rot[id] = rot;
}

static void InitPhysicsTest2(PhysicsBodies* b)
{
	SetPhysicsObject(b, 0, V2(420.f, 120.f), V2(20.f, 20.f), V2(0.f, 0.f), 0.f, 0.4f);
	SetPhysicsObject(b, 1, V2(444.f, 130.f), V2(20.f, 20.f), V2(0.f, 0.f), 1.f, 0.0f);
	// SetPhysicsObject(b, 0, V2(30.f, 30.f), V2(20.f, 20.f), V2(0.f, 0.f), 0.f, 0.1f);
	// SetPhysicsObject(b, 0, V2(30.f, 30.f), V2(20.f, 20.f), V2(0.f, 0.f), 0.f, 0.1f);
	// SetPhysicsObject(b, 0, V2(30.f, 30.f), V2(20.f, 20.f), V2(0.f, 0.f), 0.f, 0.1f);
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

static void physics_handleCollision(PhysicsBodies* bs, int collIndex, int bodyA, int bodyB)
{
	// c->sprites.count = 11;
	// setSprites(&c->sprites, *pos, V2(2.f, 2.f), tex->ID, 10);

	vec2* pos = bs->verticesPositions + collIndex;

	// let's calc collision normal lol
	// 2 closest points
#if 1 
	debugInfo.collisionPoint = *pos;
#endif

	float dSqrt[4];
	for (int i = 0; i < 4; ++i)
	{
		Vec2 distVec = vec2_subv(bs->verticesPositions + i + (bodyA * 4), pos);
		dSqrt[i] = distVec.x * distVec.x + distVec.y * distVec.y;
	}

	int min0 = 0;
	int min1 = 1;
	if (dSqrt[0] > dSqrt[1])
	{
		min0 = 1;
		min1 = 0;
	}

	for (int i = 2; i < 4; ++i)
	{
		if (dSqrt[i] < dSqrt[min0])
		{
			min1 = min0;
			min0 = i;
		}
		else if (dSqrt[i] < dSqrt[min1])
		{
			min1 = i;
		}
	}

	int index0 = min0;
	int index1 = min1;

	// normal is
	vec2* p1 = bs->verticesPositions + index0 + (bodyA * 4);
	vec2* p2 = bs->verticesPositions + index1 + (bodyA * 4);
	vec2 dist = vec2_subv(p1, p2);


	// calc collision normal
	Vec2* cubeCenter = &bs->pos[bodyA]; // 
	float cross3 = cubeCenter->x * dist.y - cubeCenter->y * dist.x;

	vec2 centerToColl = vec2_subv(pos, cubeCenter);
	float cross4 = centerToColl.x * dist.y - centerToColl.y * dist.x;
	// printf("c3 %f | %f \n", cross3, cross4);

	vec2_normalizeInPlace(&dist);

	if (cross4 < 0.f && cross3 < 0.f || cross4 < 0.f)
	{
		float tmpX = dist.x;
		dist.x = -dist.y;
		dist.y = tmpX;
	}
	else
	{
		float tmpX = dist.x;
		dist.x = dist.y;
		dist.y = -tmpX;
	}

	drawThisVec = dist;
	from = *pos;

	debugInfo.collisionNormal = dist;

	///////////////////////
	// resolve collision //

	{
		// calculate relative velocity
		vec2 rv = vec2_subv(bs->vel + bodyA, bs->vel + bodyB);

		// calculate relative velocity in terms of the normal direction
		float velAlongNormal = dotProduct(&rv, &dist);

		// do not resolve if velocities are separating
		if (velAlongNormal > 0)
			goto skip;

		// calculate restitution
		float e = 1.f; // min(restA, restB);

		// calculate impulse scalar 
		float mass1 = 1.f, mass2 = 1.f;
		float j = -(1 + e) * velAlongNormal;
		j /= 1.f / mass1 + 1.f / mass2;

		// apply impulse
		Vec2 impulse = vec2_mul(&dist, j);
		// vec2_mul_v(&impulse, &, &);

		float invMass1 = 1.f / mass1;
		float invMass2 = 1.f / mass2;

		Vec2 imp1 = vec2_mul(&impulse, invMass1);
		vec2 imp2 = vec2_mul(&impulse, invMass2);

		vec2_add_v(bs->acc + bodyA, bs->acc + bodyA, &imp1);
		vec2_sub_v(bs->acc + bodyB, bs->acc + bodyB, &imp2);


		// apply angular vel
		// av += 1.0f / inertia * Cross( contactVector, impulse );
		const float inertia = 1.f / 6.f;
		float aaa = pos->x * imp1.y - pos->y * imp1.x;
		float bbb = pos->x * imp2.y - pos->y * imp2.x;
		bs->angularVel[bodyA] += inertia * aaa;
		bs->angularVel[bodyB] += inertia * bbb;
	skip:
		int a;
	}
}

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
		// calculate relative velocity
#if 0
		vec2 rv = vec2_subv(bs->vel + bodyA, bs->vel + bodyB);

		float velAlongNormal = dotProduct(&rv, &dist);

		// do not resolve if velocities are separating

		/*
		if (velAlongNormal > 0)
			return;
		*/

		float e = 1.0f; // min(restA, restB);

		float mass1 = 1.f, mass2 = 1.f;
		float j = -(1 + e) * velAlongNormal;
		j /= 1.f / mass1 + 1.f / mass2;

		// impulse
		Vec2 impulse = vec2_mul(&dist, j);

		float invMass1 = 1.f / mass1;
		float invMass2 = 1.f / mass2;

		Vec2 imp1 = vec2_mul(&impulse, invMass1);
		vec2 imp2 = vec2_mul(&impulse, invMass2);

		vec2_add_v(bs->acc + bodyA, bs->acc + bodyA, &imp1);
		vec2_sub_v(bs->acc + bodyB, bs->acc + bodyB, &imp2);


		// apply angular vel
		// av += 1.0f / inertia * Cross( contactVector, impulse );
		// vec2 testVec = vec2_subv()


		vec2 test1 = vec2_subv(pos, bs->pos + bodyA);
		vec2 test2 = vec2_subv(pos, bs->pos + bodyB);

		// 
		const float inertia = 1.f / 3.f;

		// cross haha
		float aaa = test1.x * imp1.y - test1.y * imp1.x;
		float bbb = test2.x * imp2.y - test2.y * imp2.x;

		bs->angularVel[bodyA] += inertia * aaa;
		bs->angularVel[bodyB] -= inertia * bbb;

#else



		// V += (j / m)*n
		// W += (('contact') * j * n) / Ia


		// float j = 

		// (-(1 + e) * v1ab * n)
		// n * n * (1.f / mass1 + 1.f / mass2) + (('contactAP' * n)^2)/Ia + (('contactBP' * n)^2)/Ib

		float j;
		float e = 1.f;

		float upper = (-(1 + e));

		// r? 
		Vec2 contactAP = vec2_subv(collisionPoint, bodyA + bs->pos);
		Vec2 contactBP = vec2_subv(collisionPoint, bodyB + bs->pos);

		vec2 velA = bs->vel[bodyA];
		vec2 velB = bs->vel[bodyB];
		float angularA = bs->angularVel[bodyA];
		float angularB = bs->angularVel[bodyB];

		vec2 ang = V2(angularA * contactAP.y, angularA * contactAP.x);
		vec2 bng = V2(angularB * contactBP.y, angularB * contactBP.x);

		// float av = angularA * contactAP.y - angularA * contactAP.x;
		// float bv = angularB * contactBP.y - angularB * contactBP.x;

		vec2_add_v(&velA, &velA, &ang);
		vec2_add_v(&velB, &velB, &bng);


		Vec2 vab = vec2_subv(&velA, &velB);

		// float velAlongNormal = dotProduct(&vab, &dist);
		vec2 vab2 = vec2_subv(bs->vel + bodyA, bs->vel + bodyB);
		float velAlongNormal = dotProduct(&vab2, &dist);
		// float velAlongNormal = crossProduct(&vab2, &dist);

		printf("vel %f \n", velAlongNormal);
		if (velAlongNormal > 0)
			return;

		static int debugCounter = 0;
		debugCounter++;
		if (debugCounter == 3)
			debugBreak();

		printf("coll\n");

		// vab = vec2_mul(&vab, upper);                           // (...)*v1ab
		float dot = dotProduct(&vab, &normal);
		upper = upper * dot; //      dotProduct(&vab, &normal); // maybe
		// vec2_mul_s
		// vab = vec2_mul(&vab, &normal);
		// upper = dotProduct()

		float nn = dotProduct(&normal, &normal);

		float massOverOneA = 1.f / bs->m[bodyA];
		float massOverOneB = 1.f / bs->m[bodyB];

		float totalMass = massOverOneA + massOverOneB;


		float contactAPN = crossProduct(&contactAP, &normal);
		float contactBPN = crossProduct(&contactBP, &normal);
		// vec2 cApN = vec2_mulv(&contactAP, &normal);
		// vec2 cBpN = vec2_mulv(&contactAB, &normal);
		float contactAPN2 = (contactAPN*contactAPN) / bs->momentOfInertia[bodyA];
		float contactBPN2 = (contactBPN*contactBPN) / bs->momentOfInertia[bodyB];
		// TODO: / IA / IB

		// contactAPN2 /= 10.f;
		// contactBPN2 /= 10.f;

		float lower = nn * totalMass + contactAPN2 + contactBPN2;
		j = upper / lower;

		// float a = (j / (bs->m[bodyA]));
		float a = j / 1.f;
		Vec2 v2a;
		vec2_mul_s(&v2a, &normal, a);

		// float b = (-j / (bs->m[bodyB]));
		float b = -j / 1.f;
		Vec2 v2b;
		vec2_mul_s(&v2b, &normal, b);

		// vec2_add_v(bs->acc + bodyA, bs->acc + bodyA, &v2a);
		// vec2_add_v(bs->acc + bodyB, bs->acc + bodyB, &v);

		vec2_add_v(bs->acc + bodyA, bs->acc + bodyA, &v2a);
		vec2_add_v(bs->acc + bodyB, bs->acc + bodyB, &v2b);

		Vec2 jn = vec2_mul(&normal, j);

		float aAngular = crossProduct(&contactAP, &jn);
		float bAngular = crossProduct(&contactBP, &jn);

		bs->angularVel[bodyA] -= aAngular;
		bs->angularVel[bodyB] += bAngular;
#endif
		// (rap * j * n) / in
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
			vec2_mul(&mouseDiff, 0.1f);
			bodies->vel[controller->selectedBody] = mouseDiff;

			controller->dragging = false;
			controller->selectedBody = -1;

		}
		else
		{
			if (controller->current == 5);
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

#if 0
	for (int i = 0; i < bs->count; ++i) {
		float force = GRAV / bs->m[i];
		bs->acc[i].y += force * dt;

		if (bs->pos[i].y < 50.f)
		{
			bs->acc[i].y = 0.f;
			bs->vel[i].y = 0.f;
		}
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

	// Check For collisions
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

		if (i == 0)
		{
			c->sprites.count = 10;

			setSprites(&c->sprites, p1, V2(3.f, 3.f), tex->ID, 6);
			setSprites(&c->sprites, p2, V2(3.f, 3.f), tex->ID, 7);
			setSprites(&c->sprites, p3, V2(3.f, 3.f), tex->ID, 8);
			setSprites(&c->sprites, p4, V2(3.f, 3.f), tex->ID, 9);

			// do we intersect with mouse
#if 0
			int collides = pointInsideTringle(&p1, &p2, &p3, &mouse);

			if (!collides)
				collides = pointInsideTringle(&p2, &p3, &p4, &mouse);
#else
		// int collides = pointInsideRectangle(&p3, &p4, &p1, &p2, &mouse);
			int collides = pointInsideRectangle(&p1, &p2, &p4, &p3, &mouse);
#endif
			c->sprites.colors[0] = collides ? green : red;
		}
	}

	bool32 debug_collides[MAX_P_BODIES] = { 0 };

	int  outId;
	Vec2 outNormal;

	for (int ii = 0; ii < count; ++ii)
	{
		for (int jj = ii + 1; jj < count; ++jj)
		{
			if (polygonIntersection(bs, ii, jj, &outNormal, &outId))
			{
				// debug_collides[ii] = 1;
				// debug_collides[jj] = 1;

				int i = outId == ii ? ii : jj;
				int j = outId == ii ? jj : ii;

				// Hahahahahhahahahhahhaaahahahhahah
				int collIndex = 0;
				Vec2* vp = bs->verticesPositions;

				// other == 
				int other = outId == ii ? jj : ii;

				for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) // joka vertex
				{
					int collides = pointInsideRectangle(vp + (outId * 4) + 0, vp + (outId * 4) + 1,
						vp + (outId * 4) + 2,
						vp + (outId * 4) + 3, vp + (other * 4) + vertexIndex);

					if (collides)
					{
						collIndex = (other * 4) + vertexIndex;
						// printf("::: %i \n", collIndex);
						break;
					}
				}

				Vec2* d_collision = collIndex + vp;
				pushLine(d_collision->x, d_collision->y, d_collision->x + 1.f, d_collision->y + 1.f);

				// printf("and the out id is %i", outId);
				// printf("%i %i %i", i, j, collIndex);
				physics_handleCollision_2(bs, collIndex, jj, ii, &outNormal);
			}
		}
	}


	int other = 1;
	outId = 0;
	Vec2* vp = bs->verticesPositions;
	for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) // joka vertex
	{
		int collides = pointInsideRectangle(vp + (outId * 4) + 0, vp + (outId * 4) + 1, vp + (outId * 4) + 2,
			vp + (outId * 4) + 3, vp + (other * 4) + vertexIndex);

		if (collides)
		{
			int collIndex = (other * 4) + vertexIndex;
			printf("::: %i \n", collIndex);
			debug_collides[outId] = 1;
			// debug_collides[jj] = 1;
			from = *(vp + collIndex);
			drawThisVec = V2(1.f, 1.f);
			drawThisVec = vec2_addv(&from, &drawThisVec);
			break;
		}
	}


#if 0
	// box vs box collision
	int coll = 0;
	Vec2* vp = bs->verticesPositions;
	for (int iIndex = 0; iIndex < count * 4; iIndex += 4)
	{
		coll = -1;
		for (int jIndex = iIndex + 4; jIndex < count * 4; jIndex += 4)
		{
			// collidee ko nain pain
			// int vertIndex = physics_checkCollision(bs, iIndex, jIndex);

#if 0
			for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) // joka vertex
			{
				int collides = pointInsideRectangle(vp + iIndex + 0, vp + iIndex + 1, vp + iIndex + 3,
					vp + iIndex + 2, vp + jIndex + vertexIndex);

				if (collides)
				{
					coll = jIndex + vertexIndex;
					// handle collision
					physics_handleCollision(bs, coll, iIndex / 4, jIndex / 4);
					debug_collides[iIndex / 4] = 1;
					goto skip_coll;
					break;
}
		}
#endif
#if 0
			for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) // joka vertex
			{
				int collides = pointInsideRectangle(vp + jIndex + 0, vp + jIndex + 1, vp + jIndex + 3,
					vp + jIndex + 2, vp + iIndex + vertexIndex);

				if (collides)
				{
					coll = iIndex + vertexIndex;
					// handle collision
					physics_handleCollision(bs, coll, jIndex / 4, iIndex / 4);
					debug_collides[jIndex / 4] = 1;
					break;
				}
			}
#endif
		skip_coll:
			(void)0;
	}
}
#endif

	for (int i = 0; i < ArrayCount(debug_collides); ++i) {
		bool32 coll = debug_collides[i];
		c->sprites.colors[i] = coll ? green : red;
	}

	int coll = 0;
	if (coll)
	{
		c->sprites.count = 11;
		vec2* pos = bs->verticesPositions + coll;
		setSprites(&c->sprites, *pos, V2(2.f, 2.f), tex->ID, 10);

		// let's calc collision normal lol
		// 2 closest points

		float dSqrt[4];
		for (int i = 0; i < 4; ++i)
		{
			Vec2 distVec = vec2_subv(bs->verticesPositions + i, pos);
			dSqrt[i] = distVec.x * distVec.x + distVec.y * distVec.y;
		}

		int min0 = 0;
		int min1 = 1;
		if (dSqrt[0] > dSqrt[1])
		{
			min0 = 1;
			min1 = 0;
		}

		for (int i = 2; i < 4; ++i)
		{
			if (dSqrt[i] < dSqrt[min0])
			{
				min1 = min0;
				min0 = i;
			}
			else if (dSqrt[i] < dSqrt[min1])
			{
				min1 = i;
			}
		}

		// two closest

		// TODO: doesn't even work in all cases xd 
//		int index0 = dSqrt[0] < dSqrt[1] ? (dSqrt[0] < dSqrt[2] ? 0 : 2) : (dSqrt[1] < dSqrt[2] ? 1 : 2);
//		int index1 = dSqrt[1] < dSqrt[2] ? (dSqrt[1] < dSqrt[3] ? 1 : 3) : (dSqrt[2] < dSqrt[3] ? 2 : 3);
		int index0 = min0;
		int index1 = min1;


		// normal is
		vec2* p1 = bs->verticesPositions + index0;
		vec2* p2 = bs->verticesPositions + index1;


		vec2 dist = vec2_subv(p1, p2);


		Vec2* cubeCenter = &bs->pos[0];
		float cross3 = cubeCenter->x * dist.y - cubeCenter->y * dist.x;

		vec2 centerToColl = vec2_subv(pos, cubeCenter);
		float cross4 = centerToColl.x * dist.y - centerToColl.y * dist.x;
		printf("c3 %f | %f \n", cross3, cross4);

		vec2_normalizeInPlace(&dist);

		//float dot = dotProduct(p1, p2);
		//float cross  = p1->x * dist.y - p1->y * dist.x;
		//float scros2 = p2->x * dist.y - p2->y * dist.x;
	//	printf("c: %f, %f\n", cross, scros2);


		if (cross4 < 0.f && cross3 < 0.f || cross4 < 0.f)
		{
			float tmpX = dist.x;
			dist.x = -dist.y;
			dist.y = tmpX;
		}
		else
		{
			float tmpX = dist.x;
			dist.x = dist.y;
			dist.y = -tmpX;
		}

		// Vec2 something = vec2_addv(dist);
		drawThisVec = dist;
		from = *pos;

		// printf("dist.xy %f %f\n", dist.x, dist.y);
		// printf("dist: %f | %f | %f | %f  :: %i, %i \n", dSqrt[0], dSqrt[1], dSqrt[2], dSqrt[3], min0, min1);

		setSprites(&c->sprites, *p1, V2(4.f, 4.f), tex->ID, 11);
		setSprites(&c->sprites, *p2, V2(4.f, 4.f), tex->ID, 12);
		c->sprites.count = 13;

		// printf("unit: %f %f\n", dist.x, dist.y);


		///////////////////////
		// resolve collision //

		{
			// calculate relative velocity
			vec2 rv = vec2_subv(bs->vel + 0, bs->vel + 1);

			// calculate relative velocity in terms of the normal direction
			float velAlongNormal = dotProduct(&rv, &dist);

			// do not resolve if velocities are separating
//			if (velAlongNormal > 0)
//				goto skip;

			// calculate restitution
			float e = 1.f; // min(restA, restB);

			// calculate impulse scalar 
			float mass1 = 1.f, mass2 = 1.f;
			float j = -(1 + e) * velAlongNormal;
			j /= 1.f / mass1 + 1.f / mass2;

			// apply impulse
			Vec2 impulse = vec2_mul(&dist, j);
			// vec2_mul_v(&impulse, &, &);

			float invMass1 = 1.f / mass1;
			float invMass2 = 1.f / mass2;

			Vec2 imp1 = vec2_mul(&impulse, invMass1);
			vec2 imp2 = vec2_mul(&impulse, invMass2);

			vec2_add_v(bs->acc + 0, bs->acc + 0, &imp1);
			vec2_sub_v(bs->acc + 1, bs->acc + 1, &imp2);


			// apply angular vel


			// av += 1.0f / inertia * Cross( contactVector, impulse );
			const float inertia = 1.f / 6.f;
			float aaa = pos->x * imp1.y - pos->y * imp1.x;
			float bbb = pos->x * imp2.y - pos->y * imp2.x;
			bs->angularVel[0] += inertia * aaa;
			bs->angularVel[1] += inertia * bbb;

		skip:
			int a;
		}

	}



	bs->angularVel[4] = 0.f;
	bs->rot[4] = 0.f;
	bs->acc[4] = V2(0.f, 0.f);
	// move
	vec2_add_vs(bs->vel, bs->vel, bs->acc, bs->count);
	// vec2_add_vs(bs->pos, bs->pos, bs->vel, bs->count);

	for (int i = 0; i < count; ++i)
	{
		vec2* pos = bs->pos + i;
		vec2* vel = bs->vel + i;

		vec2_add_v(pos, pos, vel);

		if (pos->x < 0.f || pos->x > 2000.f)
			vel->x = -vel->x;

		if (pos->y < 50.f || pos->y > 500.f)
			vel->y = -vel->y;
	}


	// angularVelocity += torque * (1.0f / momentOfInertia) * dt
	// position += velocity * dt
	// orient += angularVelocity * dt

	for (int i = 0; i < count; ++i)
	{
		const float momentOfInertia = 1.f;
		bs->angularVel[i] += bs->torque[i] * (1.0f / momentOfInertia) * dt;
		bs->rot[i] += (bs->angularVel[i] * dt); // dt
	}


	}

static void drawBodies(GraphicsContext* c, GameState* state, int count)
{
	//	PhysicsBody* bs = state->bodies;
	for (int i = 0; i < count; ++i)
	{
	}

	// draw some force arrows maybe


	vec2 unit = vec2_mul(&drawThisVec, 10.f);
	vec2 destination = vec2_addv(&from, &unit);
	drawLine(c, &from, &destination, 0xFFFFFFFF);


	unit = vec2_mul(&debugInfo.collisionNormal, 10.f);
	destination = vec2_addv(&debugInfo.collisionPoint, &unit);
	drawLine(c, &debugInfo.collisionPoint, &destination, 0xFFFFFFFF);

	drawLine(c, &debugInfo.start, &debugInfo.end, 0xFFFFFFFF);


	for (int i = 0; i < d_count; ++i)
	{
		drawLine(c, &debug_lines[i].start, &debug_lines[i].end, 0xFFFFFFFF);
	}
	d_count = 0;

	Vec2 test1 = V2(50.f, 80.f);
	Vec2 test2 = V2(25.f, 40.f);
	drawLine(c, &test1, &test2, 0xFFFFFFFF);
	drawLine(c, &test1, &test2, 0xFFFFFFFF);
	drawLine(c, &test1, &test2, 0xFFFFFFFF);
	drawLine(c, &test1, &test2, 0xFFFFFFFF);

}

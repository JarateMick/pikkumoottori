#include "Physics.h"

static void testPhysicsBodies(GameState* state, GraphicsContext* c, int count)
{
	// init stuff
	PhysicsBodies* bs = state->bodies;
	for (int i = 0; i < count; ++i)
	{
		bs->pos[i] = V2(i * 25.f, 150.f);
		bs->size[i] = V2(20.f, 20.f);
		bs->m[i] = 5.f;
	}

	memset(bs->acc, 0, sizeof(Vec2) * MAX_P_BODIES);
	memset(bs->vel, 0, sizeof(Vec2) * MAX_P_BODIES);
	memset(bs->rot, 0, sizeof(float) * MAX_P_BODIES);

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

static float dotProduct(Vec2* a, Vec2* b)
{
	return a->x * b->x + a->y * b->y;
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

static int pointInsideRectangle(Vec2* a, Vec2* b, Vec2* c, Vec2* d, Vec2* point)
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

static void updateBodies(GameState* state, GraphicsContext* c, Vec2 mouse, float dt)
{
	PhysicsBodies* bs = state->bodies;
	int count = bs->count;

	// Compute Forces
	// gravity
	for (int i = 0; i < bs->count; ++i) {
		float force = GRAV / bs->m[i];
		bs->acc[i].y += force * dt;

		if (bs->pos[i].y < 50.f)
		{
			bs->acc[i].y = 0.f;
			bs->vel[i].y = 0.f;
		}
	}


	bs->rot[0] += 0.001f; // aka grad

	// Integrate velocitoies


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
		bs->verticesPositions[i * 4 + 2] = p3;
		bs->verticesPositions[i * 4 + 3] = p4;

		if (i == 0)
		{
			c->sprites.count = 10;

			setSprites(&c->sprites, p1, V2(3.f, 3.f), tex->ID, 6);
			setSprites(&c->sprites, p2, V2(3.f, 3.f), tex->ID, 7);
			setSprites(&c->sprites, p3, V2(3.f, 3.f), tex->ID, 8);
			setSprites(&c->sprites, p4, V2(3.f, 3.f), tex->ID, 9);

			// do we intersect with mouse
			// 



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

	// box vs box collision

	bs->pos[1] = mouse;
	bs->vel[1] = V2(0.f, 0.f);
	bs->acc[1] = V2(0.f, 0.f);

	int coll = 0;
	for (int i = 0; i < 4; ++i)
	{
		int collides = pointInsideRectangle(bs->verticesPositions + 0, bs->verticesPositions + 1,
			bs->verticesPositions + 3, bs->verticesPositions + 2, bs->verticesPositions + 4 + i);

		if (collides)
		{
			coll = i + 4;
			break;
		}
	}

	if (coll)
	{
		c->sprites.count = 11;
		vec2* pos = bs->verticesPositions + coll;
		setSprites(&c->sprites, *pos, V2(3.f, 3.f), tex->ID, 10);

		// let's calc collision normal lol
		// 2 closest points

		float dSqrt[4];
		for (int i = 0; i < 4; ++i)
		{
			dSqrt[i] = pos->x * bs->verticesPositions[i].x + pos->y * bs->verticesPositions[i].y;
		}

		int min0 = 0;
		int min1 = 1;
		if (dSqrt[0] < dSqrt[1])
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
		vec2_normalizeInPlace(&dist);

		// normal vector yahoo
		float tmpX = dist.x;
		dist.x = -dist.y;
		dist.y = tmpX;

		drawThisVec = dist;
		from = *pos;
	}


	c->sprites.colors[1] = coll ? green : red;




	// move
	vec2_add_vs(bs->vel, bs->vel, bs->acc, bs->count);
	vec2_add_vs(bs->pos, bs->pos, bs->vel, bs->count);


	// collide

	// resolve
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
}

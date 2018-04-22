#pragma once

// vector 2 

typedef struct Vec2 Vec2;

struct Vec2
{
	float x;
	float y;

	//ignore Vec2& operator+=(const Vec2& rhs);
	//ignore Vec2& operator-=(const Vec2& rhs);
	//ignore friend Vec2 operator+(Vec2 lhs, const Vec2& rhs);
	//ignore friend Vec2 operator-(Vec2 lhs, const Vec2& rhs);
	//
	//// ignore Vec2 operator+=(const float rhs);
	//
	//ignore Vec2& operator*=(const float s);
	//ignore Vec2& operator*=(const int s);
	//ignore Vec2& operator/=(const float s);
	//ignore Vec2& operator/=(const int s);
	//
	//ignore Vec2 operator*(const float s);
	//ignore Vec2 operator*(const int s);
	//
	//ignore Vec2 operator/(const float s);
	//ignore Vec2 operator/(const int s);
	//
	//ignore float length();
	//
	//ignore void normalizeInPlace();
	//ignore void safeNormalizeInPlace();
	//ignore Vec2 normalize();
};
typedef Vec2 vec2;

static inline Vec2 V2(float x, float y)
{
	Vec2 result = { x, y };
	return result;
}

#define VEC2_0 V2(0.f, 0.f)

static inline float dotProduct(Vec2* a, Vec2* b)
{
	return a->x * b->x + a->y * b->y;
}

static inline float crossProduct(Vec2* a, Vec2* b)
{
	return a->x * b->y - a->y * b->x;
}

static inline float pow2(float x)
{
	return x * x;
}

static inline Vec2 CrossProductVS(const Vec2* a, float s)
{
	return V2(-s * a->y, s * a->x);
}

static inline Vec2 CrossProductSV(float s, const Vec2* a)
{
	return V2(-s * a->y, s * a->x);
}

static inline Vec2 turn(Vec2* a)
{
	return V2(-a->y, a->x);
}

static inline Vec2 vec2_subv(Vec2* a, Vec2* b)
{
	Vec2 result;
	result.x = a->x - b->x;
	result.y = a->y - b->y;
	return result;
}

static inline Vec2 vec2_div(Vec2* a, float s)
{
	Vec2 result;
	result.x = a->x / s;
	result.y = a->y / s;
	return result;
}

static inline Vec2 vec2_add(Vec2* a, float s)
{
	Vec2 result;
	result.x = a->x + s;
	result.y = a->y + s;
	return result;
}

static inline void vec2_add_s(Vec2* dest, Vec2* right,  float s)
{
	dest->x = right->x + s;
	dest->y = right->y + s;
}

static inline void vec2_sub_s(Vec2* dest, Vec2* right,  float s)
{
	dest->x = right->x + s;
	dest->y = right->y + s;
}

static inline Vec2 vec2_mul(Vec2* a, float s)
{
	Vec2 result;
	result.x = a->x * s;
	result.y = a->y * s;
	return result;
}

static inline Vec2 vec2_mulv(Vec2* a, Vec2* b)
{
	Vec2 result;
	result.x = a->x * b->x;
	result.y = a->y * b->y;
	return result;
}

static inline void vec2_mul_s(Vec2* dest, Vec2* right, float s)
{
	dest->x = right->x * s;
	dest->y = right->y * s;
}

static inline void vec2_div_s(Vec2* dest, Vec2* right, float s)
{
	dest->x = right->x / s;
	dest->y = right->y / s;
}

//static inline void vec2_add_v(Vec2* dest, Vec2* left, Vec2* right)
//{
//	dest->x = left->x + right->x;
//	dest->y = left->y + right->y;
//}

static inline void vec2_mul_v(Vec2* dest, Vec2* left, Vec2* right)
{
	dest->x = left->x * right->x;
	dest->y = left->y * right->y;
}

static inline void vec2_div_v(Vec2* dest, Vec2* left, Vec2* right)
{
	dest->x = left->x / right->x;
	dest->y = left->y / right->y;
}


static inline Vec2 vec2_addv(Vec2* a, Vec2* b)
{
	Vec2 result;
	result.x = a->x + b->x;
	result.y = a->y + b->y;
	return result;
}

static inline void vec2_add_v(Vec2* dest, Vec2* r, Vec2* l)
{
	dest->x = r->x + l->x;
	dest->y = r->y + l->y;
}

static inline void vec2_add_vs(Vec2* dest, vec2* r, vec2* l, int c)

{
	for (int i = 0; i < c; ++i)
	{
		vec2_add_v(dest + i, r + i, l + i);
	}
}

static inline void vec2_sub_v(Vec2* dest, Vec2* r, Vec2* l)
{
	dest->x = r->x - l->x;
	dest->y = r->y - l->y;
}

static inline float vec2_len(Vec2* a)
{
	return sqrt(a->x * a->x + a->y * a->y);
}

static inline void vec2_normalizeInPlace(Vec2* v)
{
	float len = vec2_len(v);
	if (len != 0.f)
	{
		v->x /= len;
		v->y /= len;
	}
}

#if 0
static Vec2 rotateVec(Vec2* vec, float theta)
{
	float cs = cos(theta);
	float sn = sin(theta);

	float x = vec->x * cs - vec->y * sn;
	float y = vec->x * sn + vec->y * cs;

	return 	{ x, y };
}

Vec2 Vec2::operator/(const float s)
{
	return { x / s, y / s };
}

Vec2 Vec2::operator/(const int s)
{
	return { x / s, y / s };
}

Vec2 Vec2::operator*(const float s)
{
	return { x * s, y * s };
}

Vec2 Vec2::operator*(const int s)
{
	return { x * s, y * s };
}


void Vec2::normalizeInPlace()
{
	float x = (this->x);
	float y = (this->y);
	float l = sqrt(x * x + y * y);

	this->x /= l;
	this->y /= l;
}

void Vec2::safeNormalizeInPlace()
{
	if (this->x == 0.f && this->y == 0.f)
		return;

	float x = (this->x);
	float y = (this->y);
	float l = sqrt(x * x + y * y);

	this->x /= l;
	this->y /= l;
}
// täällä ei pelätä vaaroja!
Vec2 Vec2::normalize()
{
	float x = (this->x);
	float y = (this->y);
	float l = sqrt(x * x + y * y);

	return { x / l, y / l };
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;		y += rhs.y;
	return *this;
}

Vec2& Vec2::operator-=(const Vec2& rhs)
{
	x -= rhs.x;		y -= rhs.y;
	return *this;
}

Vec2 operator+(Vec2 lhs, const Vec2& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

Vec2 operator-(Vec2 lhs, const Vec2& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}



Vec2& Vec2::operator*=(const float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vec2& Vec2::operator*=(const int s)
{
	x *= s;
	y *= s;
	return *this;
}
Vec2& Vec2::operator/=(const float s)
{
	x /= s;
	y /= s;
	return *this;
}

Vec2& Vec2::operator/=(const int s)
{
	x /= s;
	y /= s;
	return *this;
}

float Vec2::length()
{
	float x = abs(this->x);
	float y = abs(this->y);
	return sqrt(x * x + y * y);
	}

#endif
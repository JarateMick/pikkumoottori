#pragma once

// vector 2 


typedef struct
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
} Vec2;
typedef Vec2 vec2;

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
	result.x = a->x /= s;
	result.y = a->y /= s;
	return result;
}

static inline Vec2 vec2_add(Vec2* a, float s)
{
	Vec2 result;
	result.x = a->x + s;
	result.y = a->y + s;
	return result;
}

static inline Vec2 vec2_addv(Vec2* a, Vec2* b)
{
	Vec2 result;
	result.x = a->x + b->x;
	result.y = a->y + b->y;
	return result;
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
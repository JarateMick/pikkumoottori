#pragma once 

typedef struct 
{
	float x, y, z;
} vec3;

#if 0
float vec3::operator*(const vec3& rhs)
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

vec3 operator-(vec3 lhs, const vec3& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

vec3 vec3::operator/(const float s)
{
	return { x / s, y / s, z / s };
}

vec3 vec3::operator/(const int s)
{
	return { x / s, y / s, z / s };
}


vec3& vec3::operator+=(const vec3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

vec3& vec3::operator-=(const vec3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

void vec3::normalizeInPlace()
{
	float x = (this->x);
	float y = (this->y);
	float z = (this->z);
	float l = sqrt(x * x + y * y + z * z);

	if (l != 0)
	{
		this->x /= l;
		this->y /= l;
		this->z /= l;
	}
}

vec3 vec3::normalize() const
{
	float x = (this->x);
	float y = (this->y);
	float z = (this->z);
	float l = sqrt(x * x + y * y + z * z);

	if (l != 0)
	{
		x /= l;
		y /= l;
		z /= l;
		return { x, y, z };
	}
	return { 0.f, 0.f, 0.f };
}

inline vec3 operator+(const vec3 & lhs, const vec3 & rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

vec3 vec3::operator*(const float s)
{
	return { x * s, y * s, z * s };
}
vec3 vec3::operator*(const int s)
{
	return { x * s, y * s, z * s };
}

#endif
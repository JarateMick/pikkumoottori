#line 1 "..\\src\\application\\game.cpp"

#line 1 "i:\\dev\\motors\\pikkumoottori\\src\\application\\game.h"
#pragma once

// #include <chrono>
#line 1 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"
#pragma once

















#line 20 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"








#line 29 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"


// plug assert to playback on windows ?








#line 41 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"























#line 1 "../inc\\vec2.h"
#pragma once

// vector 2 

 struct Vec2
{
float x;
float y;

 Vec2& operator+=(const Vec2& rhs);
 Vec2& operator-=(const Vec2& rhs);
 friend Vec2 operator+(Vec2 lhs, const Vec2& rhs);
 friend Vec2 operator-(Vec2 lhs, const Vec2& rhs);

// ignore Vec2 operator+=(const float rhs);

 Vec2& operator*=(const float s);
 Vec2& operator*=(const int s);
 Vec2& operator/=(const float s);
 Vec2& operator/=(const int s);

 Vec2 operator*(const float s);
 Vec2 operator*(const int s);

 Vec2 operator/(const float s);
 Vec2 operator/(const int s);

 float length();

 void normalizeInPlace();
 void safeNormalizeInPlace();
 Vec2 normalize();
};


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
// tÃ¤Ã¤llÃ¤ ei pelÃ¤tÃ¤ vaaroja!
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

#line 65 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"
#line 1 "../inc\\vec3.h"
#pragma once 

 struct vec3
{
	float x;
	float y; float z;


	 vec3& operator+=(const vec3& rhs);
	 vec3& operator-=(const vec3& rhs);
	 friend vec3 operator+(const vec3& lhs, const vec3& rhs);
	 vec3 operator/(const float s);
	 vec3 operator/(const int s);

	 friend vec3 operator-(vec3 lhs, const vec3& rhs);


	 float operator*(const vec3& rhs);

	 vec3 operator*(const float s);
	 vec3 operator*(const int s);
	void normalizeInPlace();
	vec3 normalize() const;
};

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

#line 66 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"
#line 1 "../inc\\vec4.h"
#pragma once 

 struct vec4
{
    float x;
    float y;
    float w;
    float h;
};
#line 67 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"



#line 1 "i:\\dev\\motors\\pikkumoottori\\src\\graphics/graphicsF.h"
#pragma once

struct CameraState
{
	Vec2  position;
	float scale;
	bool needUpdate;
};

struct Sprites
{
	Vec2* positions;
	Vec2* sizes;
	vec4* uvs;
	vec4* colors;
	int* ids;
	float* rotation;

	unsigned int count;
	// rotation jne
};

/// hahaa
struct GraphicsContext
{
	bool initted;
	bool updateViewPort;
	CameraState camera;
	Sprites sprites;
	void* window;
};

void zoom(CameraState* camera, float factor)
{
	camera->scale += factor;
	camera->needUpdate = true;
	if (camera->scale < 0.01f)
		camera->scale = 0.02f;

}

void translate(CameraState* camera, const Vec2& position)
{
	camera->needUpdate = true;
	camera->position += position;
}

// camera front
// sprite front
// 
#line 71 "i:\\dev\\motors\\pikkumoottori\\src\\application\\../platform.h"

struct Controller
{
	enum CameraKeys
	{
		w, a, s, d, ctrl, q, e,
		size,
	};

	bool jump;
	bool cameraMovement[size];
};

struct EngineContext
{
	float dt;
	Controller controller;
	GraphicsContext context;
	void* imguiContext;

	Vec2 windowDims;
};


typedef void init_game(EngineContext* engine);


typedef void update_Game(EngineContext* engine);


typedef void draw_Game(EngineContext* engine);

#line 5 "i:\\dev\\motors\\pikkumoottori\\src\\application\\game.h"

extern "C" __declspec(dllexport) void initGame(EngineContext* engine);
extern "C" __declspec(dllexport) void updateGame(EngineContext* engine);
extern "C" __declspec(dllexport) void drawGame(EngineContext* engine);

#line 3 "..\\src\\application\\game.cpp"


extern "C" __declspec(dllexport) void initGame(EngineContext* engine)
{
	printf("game init\n");

	GraphicsContext* c = &engine->context;

	zoom(&c->camera, 1.f);
	
}

extern "C" __declspec(dllexport) void updateGame(EngineContext* engine)
{
//	LOGI("update \n");
}

extern "C" __declspec(dllexport) void drawGame(EngineContext* engine)
{
//	LOGI("render \n");
}

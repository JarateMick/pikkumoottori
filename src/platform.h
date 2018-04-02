#pragma once
#define ASSERTION_ENABLED 1

#define true 1
#define false 0

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define  LOGI(...) printf(__VA_ARGS__)
#define  LOGW(...)  printf(__VA_ARGS__)
#define  LOGE(...)  printf(__VA_ARGS__)	

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT extern "C" 
#endif

#if ASSERTION_ENABLED == 1
#ifdef _WIN32
#define debugBreak() __debugbreak();
#else
// #include <assert.h>	
// #define debugBreak() assert(false); // TODO: android break to debugger
#define debugBreak()
#endif


// plug assert to playback on windows ?
#define ASSERT(expr)   \
	if (expr) { } \
	else \
	{    \
		debugBreak();                 \
	}
#else
#define ASSERT(expr) 
#endif

#if 0
#if  defined(_CHRONO_) || defined(__EMSCRIPTEN__)
#define NAME(ending) (func_timing##ending)

#ifndef __EMSCRIPTEN__
#define START_TIMING() \
	auto NAME(1)(std::chrono::high_resolution_clock::now()); \

#define END_TIMING() \
	auto NAME(2)(std::chrono::high_resolution_clock::now());	\
	auto elapsedTime(NAME(2) - NAME(1));						\
	printf("Time: %f\n", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(elapsedTime).count()  );

#define START_TIMING2() \
	auto NAME(3)(std::chrono::high_resolution_clock::now()); \

#define END_TIMING2() \
	auto NAME(4)(std::chrono::high_resolution_clock::now());	\
	auto elapsedTime2(NAME(4) - NAME(3));						\
	printf("Time: %f\n", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(elapsedTime2).count()  );
#else
#define START_TIMING2()
#define END_TIMING2()

#endif
#endif
#endif

typedef struct
{
	float v[16];
} mat4;

#define introspect() 
#define ignore
#include <vec2.h>
#include <vec3.h>
#include <vec4.h>
#include <3rdparty/linmath.h>


#undef ignore
#undef introspect

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

typedef float real32;
typedef double real64;


#include "graphics/graphicsF.h"

enum CameraKeys
{
	w, a, s, d, ctrl, q, e,
	size,
};

typedef struct
{

	bool32 jump;
	bool32 cameraMovement[size];

	bool32 mouseDown;
	bool32 mouseLastFrame;
	Vec2 mousePos;
	Vec2 mouseWorldPos;

	int32 buttons;
	int32 lastFrame;
	int32 mouseWheel;
} Controller;


// Scancode_A - 4 == Key_A

// 0123
// 0000 

enum keys_enum
{
	Key_A,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,

	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_MAX,
};


// toggle: number ^= 1&UL << x;
static void controller_pressKey(Controller* c, uint32 key)
{
	c->buttons |= 1UL << key;
}
static void controller_unpressKey(Controller* c, uint32 key)
{
	c->buttons &= ~(1UL << key);
}

static void controller_updateKeys(Controller* c)
{
	c->lastFrame = c->buttons;
	c->mouseWheel = 0;
	c->mouseLastFrame = c->mouseDown;
}

typedef struct
{
	float dt;
	Controller controller;
	GraphicsContext context;
	void* imguiContext;

	bool32 reloadGraphics;
	Vec2 windowDims;

	int runToFrame;
	int currentFrame;
	float simulationAdditionalDt;
	int freeRun;
} EngineContext;

static bool32 isKeyDown(EngineContext* e, uint32 key)
{
	return ((e->controller.buttons >> key) & 1U) == 1;
}
static bool32 isKeyPressed(EngineContext* e, uint32 key)
{
	bool32 thisFrame = (e->controller.buttons   >> key) & 1U;
	bool32 lastFrame = (e->controller.lastFrame >> key) & 1U;
	return thisFrame && !lastFrame;
}

static bool32 isMousePressed(EngineContext* e, uint32 key)
{
	return (e->controller.mouseDown) && !e->controller.mouseLastFrame;
}


typedef struct
{
	bool32 isInitialized;
	void* memory;
	uint64 size;
} Memory;

#define INIT_GAME(name) void name(EngineContext* engine, Memory* mem)
typedef INIT_GAME(init_game);

#define UPDATE_GAME(name) void name(EngineContext* engine, Memory* mem)
typedef UPDATE_GAME(update_Game);

#define DRAW_GAME(name) void name(EngineContext* engine, Memory* mem)
typedef DRAW_GAME(draw_Game);


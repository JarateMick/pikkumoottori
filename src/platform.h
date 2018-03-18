#pragma once
#define ASSERTION_ENABLED 1

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define  LOGI(...) printf(__VA_ARGS__)
#define  LOGW(...)  printf(__VA_ARGS__)
#define  LOGE(...)  printf(__VA_ARGS__)	

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
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

#ifdef _CHRONO_
#define NAME(ending) (func_timing##ending)

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
#endif

#define introspect() 
#define ignore
#include <vec2.h>
#include <vec3.h>
#include <vec4.h>
#undef ignore
#undef introspect

#include <stdint.h>
typedef uint8_t  uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;


#include "graphics/graphicsF.h"

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

	bool reloadGraphics;
	Vec2 windowDims;
};

struct Memory
{
	bool isInitialized;
	void* memory;
	uint64 size;
};

#define INIT_GAME(name) void name(EngineContext* engine, Memory* mem)
typedef INIT_GAME(init_game);

#define UPDATE_GAME(name) void name(EngineContext* engine, Memory* mem)
typedef UPDATE_GAME(update_Game);

#define DRAW_GAME(name) void name(EngineContext* engine, Memory* mem)
typedef DRAW_GAME(draw_Game);


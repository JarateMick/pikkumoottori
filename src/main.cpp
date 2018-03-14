#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

#include <SDL2/SDL.h>
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1

#include <windows.h>
#include <sys/stat.h>
#include <chrono>

#include "platform.h"
#include "hotreload.cpp"

// #include <glad/glad.h> #include "glad.c"


int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);


	const char* windowName = "hello sdl";
	int width = 500; int height = 500;

#ifndef __EMSCRIPTEN__
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 2);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 3);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 2);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 2);
#endif
#ifdef DEBUG_OPENGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#ifdef __EMSCRIPTEN__
	SDL_Window* window = SDL_CreateWindow(windowName, 0, 0, width, height, flags);
#else
	SDL_Window* window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
#endif

	if (!window)
	{
		// debugBreak();
		printf("failed to window \n");
		// std::terminate();
		return false;
	}

	SDL_GLContext glContex = SDL_GL_CreateContext(window);

	// auto rdr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if (!glContex)
	{
		// debugBreak();
		printf("failed to context \n");
		// std::terminate();
		return false;
	}


	SDL_GL_MakeCurrent(window, glContex);
	SDL_GL_SetSwapInterval(0);

#ifndef __EMSCRIPTEN__

#endif
	bool quit = false;

	ApplicationFunctions app = { };
	initializeApplication(&app, "game");

	ApplicationFunctions graphics = { };
	initializeApplication(&graphics, "graphics");

	EngineContext engine = { };
	engine.windowDims.x = width;
	engine.windowDims.y = height;

	if (app.gameInitPtr)
	{
		app.gameInitPtr(&engine);
	}
	
	if (graphics.gameDrawPtr)
	{
		graphics.gameInitPtr(&engine);
	}


	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
			{
				quit = true;
			} break;
			default:
				break;
			}
		}

		update(&app);

		if (app.gameUpdatePtr)
		{
			app.gameUpdatePtr(&engine);
		}

		if (app.gameDrawPtr)
		{
			app.gameDrawPtr(&engine);
		}

		if (graphics.gameUpdatePtr)
		{
			graphics.gameDrawPtr(&engine);
		}

		SDL_GL_SwapWindow(window);
	}

	return 0;
}


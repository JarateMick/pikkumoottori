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

#include "glad/glad.h"
#include "glad.c"

#include "Imgui/imgui.h"
#include "Imgui/imgui_demo.cpp"
#include "Imgui/imgui_impl_sdl_gl3.cpp"




int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);


	const char* windowName = "hello sdl";
	int width = 1024; int height = 860;

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

	if (!gladLoadGL())
	{
		printf("failed to initaliaze GLAD\n");
		ASSERT(false);
	}


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

	ImGui_ImplSdlGL3_Init(window);
	bool show_demo_window = true;

	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSdlGL3_ProcessEvent(&e);

			switch (e.type)
			{
			case SDL_QUIT:
			{
				quit = true;
			} break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				bool down = e.key.state;
				
				switch (e.key.keysym.scancode)
				{
				case SDL_SCANCODE_W:
					engine.controller.cameraMovement[0] = down; break;
				case SDL_SCANCODE_A:
					engine.controller.cameraMovement[1] = down; break;
				case SDL_SCANCODE_S:
					engine.controller.cameraMovement[2] = down; break;
				case SDL_SCANCODE_D:
					engine.controller.cameraMovement[3] = down; break;
				default: 
					break;
				}
			}
			default:
				break;
			}
		}
		ImGui_ImplSdlGL3_NewFrame(window);

		if (ImGui::Button("Demo Window"))                       // Use buttons to toggle our bools. We could use Checkbox() as well.
			show_demo_window ^= 1;

		if (show_demo_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("hello world");

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

		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}

	return 0;
}


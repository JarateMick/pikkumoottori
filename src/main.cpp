#define _CRT_SECURE_NO_WARNINGS 1
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1

#include <stdio.h>
#include <SDL2/SDL.h>

#define RF_DARRAY_IMPLEMENTATION
#define RF_DARRAY_SHORT_NAMES
#include <3rdparty/rf_darray.h>


#ifndef __EMSCRIPTEN__
#include <windows.h>
#include <sys/stat.h>
#else
#include <cstdlib>
#include <math.h>
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#endif

#include <chrono>
#include "platform.h"

struct ApplicationFunctions
{
	init_game*   gameInitPtr;
	update_Game* gameUpdatePtr;
	draw_Game*   gameDrawPtr;
	void*        handle;
	time_t       lastWriteTime;

	Memory memory;

	char initName[64];
	char updateName[64];
	char drawName[64];
	char   name[64];
};

void initMemory(ApplicationFunctions* app, uint64 size)
{
	app->memory.isInitialized = false;
	app->memory.size = size;

	void* mem = malloc(size);
	if (mem)
		app->memory.memory = mem;
	else
		ASSERT(false);
}


// #include <glad/glad.h> #include "glad.c"


#include "hotreload.cpp" // must be after appfuncs
#ifndef __EMSCRIPTEN__
#include "Imgui/imgui.h"
#include "Imgui/imgui_demo.cpp"
#include "Imgui/imgui_impl_sdl_gl3.cpp"
#include "glad/glad.h"
#include "glad.c"
#else
#include <emscripten.h>
#include <functional>

#include "graphics/graphics.c"
#include "application/game.c"

std::function<void()> loop;
static void main_loop() { loop(); }
#endif




#ifndef __EMSCRIPTEN__
int main(int argc, char* argv[])
#else
extern "C" int mainf()
#endif
{
	SDL_Init(SDL_INIT_EVERYTHING);


	const char* windowName = "Fysiikka";
	int width = 1024; int height = 860;

#ifdef __EMSCRIPTEN__ 
	width  /= 2;
	height /= 2;
#endif

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
	if (!gladLoadGL())
	{
		printf("failed to initaliaze GLAD\n");
		ASSERT(false);
	}
#endif

	bool quit = false;

	ApplicationFunctions app = { };
#ifndef __EMSCRIPTEN__
	initializeApplication(&app, "game", "initGame", "updateGame", "drawGame");
#endif

	ApplicationFunctions graphics = { };
#ifndef __EMSCRIPTEN__
	initializeApplication(&graphics, "graphics", "initGraphics", "updateGraphics", "drawGraphics");
#endif
	initMemory(&app, Megabytes(4));
	initMemory(&graphics, Megabytes(1));

#ifdef __EMSCRIPTEN__
	app.gameInitPtr   = initGame;
	app.gameUpdatePtr = updateGame;
	app.gameDrawPtr   = drawGame;

	graphics.gameInitPtr   = initGraphics;
	graphics.gameUpdatePtr = updateGraphics;
	graphics.gameDrawPtr   = drawGraphics;

	// printf("%p ", app.gameInitPtr);
	// printf("%p ", app.gameDrawPtr);
#endif


	EngineContext engine = { };
	engine.windowDims.x = (float)width;
	engine.windowDims.y = (float)height;

#ifndef __EMSCRIPTEN__
	ImGui_ImplSdlGL3_Init(window);
	bool show_demo_window = true;

	engine.imguiContext = (void*)ImGui::GetCurrentContext();
#endif

	callInit(&engine, &graphics); // INIT BEFORE APP
	callInit(&engine, &app);


	float dt = 0.f;
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;

#ifndef __EMSCRIPTEN__
	while (!quit) {
#else
	loop = [&] {
#endif
	//	auto timePoint1(std::chrono::high_resolution_clock::now());
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
#ifndef __EMSCRIPTEN__
			ImGui_ImplSdlGL3_ProcessEvent(&e);
#endif

			switch (e.type)
			{
			case SDL_QUIT:
			{
				quit = true;
			} break;
			case SDL_MOUSEMOTION:
			{
				engine.controller.mousePos.x = (float)e.motion.x;
				engine.controller.mousePos.y = (float)e.motion.y;
			} break;
			case SDL_MOUSEBUTTONDOWN:
			{
				engine.controller.mouseDown = true;
			} break;
			case SDL_MOUSEBUTTONUP:
			{
				engine.controller.mouseDown = false;
			} break;
			case SDL_MOUSEWHEEL: 
			{
				// pMouseWheelScrolled = true;
//				pMouseWheel.x = e.wheel.x;
				engine.controller.mouseWheel = e.wheel.y;
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
				case SDL_SCANCODE_LCTRL:
					engine.controller.cameraMovement[4] = down; break;
				case SDL_SCANCODE_Q:
					engine.controller.cameraMovement[5] = down; break;
				case SDL_SCANCODE_E:
					engine.controller.cameraMovement[6] = down; break;
				default:
					break;
				}


				// Real scancode >|
				SDL_Scancode scancode = e.key.keysym.scancode;

				const int sdl_to_pikkumoottori = 4;
				int code = scancode - sdl_to_pikkumoottori;

				if (code >= 0 && code < keys_enum::Key_MAX)
				{
					if (down)
						controller_pressKey(&engine.controller, code);
					else
						controller_unpressKey(&engine.controller, code);
				}

			} break;
			default:
				break;
			}
		}

#ifndef __EMSCRIPTEN__
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
		ImGui::InputInt("Runtoframe", &engine.runToFrame);

		if (ImGui::Button("Reset frames"))
		{
			engine.runToFrame = 0;
			engine.currentFrame = 0;
		}

		if (ImGui::Button("free run"))
		{
			engine.freeRun = !engine.freeRun;
		}



		if (ImGui::Button("reinit game"))
		{
			app.gameInitPtr(&engine, &app.memory);
		}

		if (ImGui::Button("reinit graphics"))
		{
			graphics.gameInitPtr(&engine, &graphics.memory);
		}


#ifndef __EMSCRIPTEN__
		update(&app);
		if (update(&graphics))
			engine.reloadGraphics = true;
#else
//		engine.dt = 16.f / 100.f; 
#endif
#endif

		callUpdate(&engine, &app);
		callUpdate(&engine, &graphics);
		callRender(&engine, &app);
		callRender(&engine, &graphics);

		controller_updateKeys(&engine.controller);

		{
			Vec2 mousePos = convertScreenToWorld(&engine.context.camera, engine.controller.mousePos, &engine.windowDims);
			engine.controller.mouseWorldPos = mousePos;
#ifndef __EMSCRIPTEN__
			ImGui::Text("Mouse (%f %f)", mousePos.x, mousePos.y);
			ImGui::Render();
#endif
		}

		SDL_GL_SwapWindow(window);

		//auto timePoint2(std::chrono::high_resolution_clock::now());
		//auto elapsedTime(timePoint2 - timePoint1);
		//float ft = { std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(elapsedTime).count() };

		//dt = ft;
		//float ftSeconds = (ft / 1000.f);
		//engine.dt = ft;

		dt = ((NOW - LAST) / (double)SDL_GetPerformanceFrequency() ); 
		engine.dt = dt;
		// LOGI("DT: %f \n", engine.dt);
		// LOGI("STILL ALIVE\n");

	};


#if __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	return 0;
#endif

	return 0;
	}


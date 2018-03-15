#define _CRT_SECURE_NO_WARNINGS 1
#include "game.h"


EXPORT INIT_GAME(initGame)
{
	LOGI("game init\n");

	GraphicsContext* c = &engine->context;

	zoom(&c->camera, 1.f);
	
}

EXPORT UPDATE_GAME(updateGame)
{
//	LOGI("update \n");
}

EXPORT DRAW_GAME(drawGame)
{
//	LOGI("render \n");
}

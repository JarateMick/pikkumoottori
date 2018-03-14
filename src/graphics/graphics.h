#pragma once

#include <chrono>
#include "../platform.h"
#include "graphicsF.h"

struct SpriteBatch
{
	unsigned int VAO;
	unsigned int buffers[5];
};

EXPORT INIT_GAME(gameInit);
EXPORT UPDATE_GAME(gameUpdate);
EXPORT DRAW_GAME(gameDraw);



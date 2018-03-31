#pragma once

#define TEXTURE_ENUM(DO) \
	DO(infantry) \
	DO(circle)\
	DO(box)\
	DO(awesomeface)\
	DO(noise)\
	DO(count)

#define MAKE_ENUM(VAR) Texture_##VAR,
typedef enum {
	TEXTURE_ENUM(MAKE_ENUM)
} TextureEnum ;

#define MAKE_STRINGS(VAR) #VAR,
const char* const TextureEnumToStr[] = {
	TEXTURE_ENUM(MAKE_STRINGS)
};

#define MAKE_MACRO_ENUM(VAR) #VAR,

#define NAMES  \
(const char*[]) { \
	TEXTURE_ENUM(MAKE_MACRO_ENUM) \
}

#undef TEXTURE_ENUM
#undef MAKE_STRINGS
#undef MAKE_ENUN
#undef MAKE_MACRO_ENUM


typedef struct 
{
	Vec2  position;
	float scale;
	bool32 needUpdate;
} CameraState;

typedef struct 
{
	Vec2* positions;
	Vec2* sizes;
	vec4* uvs;
	vec4* colors;
	int* ids;
	float* rotation;

	unsigned int count;
	// rotation jne
} Sprites;

typedef struct 
{
	int width, height;
	unsigned int ID;
	uint64_t writeTime;
} Texture2D;


typedef struct
{
	Texture2D textures[Texture_count];
	int loaded[Texture_count];
} ResourceHolder;


typedef struct 
{
	Texture2D* (*getTexture)(ResourceHolder* h, TextureEnum texture);
} GraphicsFuncs;

typedef struct
{
	Vec2* lineVertices;
	uint32* colors;
	int count;
} Lines;

/// hahaa
typedef struct GraphicsContex
{
	bool32 initted;
	bool32 updateViewPort;
	CameraState camera;
	Sprites sprites;
	void* window;

	GraphicsFuncs funcs;
	ResourceHolder resourceHolder;
	Lines lines;
} GraphicsContext;

static void drawLine(GraphicsContext* c, Vec2* a, Vec2* b, uint32 color)
{
	da_push(c->lines.lineVertices, *a);
	da_push(c->lines.lineVertices, *b);
	da_push(c->lines.colors, color);
	++(c->lines.count);
}

void zoom(CameraState* camera, float factor)
{
	camera->scale += factor;
	camera->needUpdate = true;
	if (camera->scale < 0.01f)
		camera->scale = 0.02f;

	printf("%f\n", camera->scale);
}

void translate(CameraState* camera, Vec2* position)
{
	camera->needUpdate = true;
	camera->position = vec2_addv(&camera->position, position);
}

Vec2 convertScreenToWorld(CameraState* camera, Vec2 screenCoords, Vec2* screenDims)
{
	screenCoords.y = screenDims->y - screenCoords.y;
	Vec2 dimsHalf = { screenDims->x / 2, screenDims->y / 2 };
	screenCoords = vec2_subv(&screenCoords, &dimsHalf);
	screenCoords = vec2_div(&screenCoords, camera->scale);
	screenCoords = vec2_addv(&screenCoords, &camera->position);
	return screenCoords;
}

// camera front
// sprite front
// 

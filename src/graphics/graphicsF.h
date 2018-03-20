#pragma once

#define TEXTURE_ENUM(DO) \
	DO(infantry) \
	DO(circle)\
	DO(box)\
	DO(awesomeface)\
	DO(noise)\
	DO(count)

#define MAKE_ENUM(VAR) Texture_##VAR,
enum TextureEnum {
	TEXTURE_ENUM(MAKE_ENUM)
};

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

struct Texture2D
{
	int width, height;
	unsigned int ID;
	uint64_t writeTime;
};

struct GraphicsFuncs
{
	Texture2D* (*getTexture)(TextureEnum texture);
};

/// hahaa
struct GraphicsContext
{
	bool initted;
	bool updateViewPort;
	CameraState camera;
	Sprites sprites;
	void* window;

	GraphicsFuncs funcs;
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

Vec2 convertScreenToWorld(CameraState* camera, Vec2 screenCoords, Vec2* screenDims)
{
	screenCoords.y = screenDims->y - screenCoords.y;
	screenCoords -= Vec2{ screenDims->x / 2, screenDims->y / 2 };
	screenCoords /= camera->scale;
	screenCoords += camera->position;
	return screenCoords;
}

// camera front
// sprite front
// 

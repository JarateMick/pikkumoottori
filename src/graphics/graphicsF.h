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

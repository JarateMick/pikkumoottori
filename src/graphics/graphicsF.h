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

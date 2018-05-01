#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include "TextureManager.h"
#include "IDrawable.h"

#include <vector>

#include "Camera.h"

class Renderer : protected QOpenGLFunctions_4_5_Core
{
public:
	Renderer();
	~Renderer();

	// init
	void InitShaders();
	void InitCamera(int canvasWidth, int canvasHeight);

	// Draw
	void DrawAll();

private:
	Camera camera;

	std::vector<IDrawable*> m_objects;
};


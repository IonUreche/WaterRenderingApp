#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLContext>

#include "IDrawable.h"

#include <vector>

#include "Camera.h"

#include "ScreenQuad.h"
#include "Terrain.h"
#include "WorldGizmo.h"
#include "Mesh.h"
#include "Water.h"
#include "Cube.h"
#include "Skybox.h"


class Renderer
{
public:
	Renderer(QOpenGLContext *glContext, GLuint defaultFrameBuffer);
	~Renderer();

	// init
	void InitAll();
	void InitShaders();
	void InitCamera();
	void InitLighting();
	void InitBuffers();
	void InitObjects();

	void InitFrameBuffer();

	// Update
	void Update();

	// Draw
	void DrawAll(GLuint defFrameBuffer);
	void DrawFinalPass();

	void SetCanvasSize(int canvasWidth, int canvasHeight);

	void RegisterObject(IDrawable *object);
	void UnregisterObject(IDrawable *object);

	Camera* GetCamera(){ return m_camera; }

private:

	QOpenGLContext *m_pGLContext = nullptr;
	QOpenGLFunctions_4_5_Core* f = nullptr;

	Camera *m_camera = nullptr;

	// transforms
	glm::mat4 m_mvp;
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;

	float m_time;

	int m_canvasWidth;
	int m_canvasHeight;

	std::vector<IDrawable*> m_objects;

	GLuint m_fbo[3];
	GLuint m_colorTex[3][1];
	GLuint m_depthTex[3];

	GLuint m_defaultFramebuffer;

	ScreenQuad* m_screenQuad;
	ScreenQuad* m_screenQuad2;
	ScreenQuad* m_screenQuad3;
	ScreenQuad* m_screenQuad4;

	std::vector<Cube*> m_cubes;

	Terrain* m_terrain;
	Water* m_water;
	WorldGizmo* m_gizmo;

	Skybox* m_skybox;
};


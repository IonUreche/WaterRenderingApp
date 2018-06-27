#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLContext>

#include <QTime>

#include "IDrawable.h"

#include <vector>

#include "Camera.h"

#include "ScreenQuad.h"
#include "Terrain.h"
#include "WorldGizmo.h"
#include "Mesh.h"
#include "Water.h"
#include "WaterGerstner.h"
#include "WaterIFFT.h"
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

	void SetGerstnerParams(std::vector<float> waterParams){ m_waterParams = waterParams; }
	void SetGerstnerNormalsParams(std::vector<float> wavesNormalMapData) { m_wavesNormalMapData = wavesNormalMapData; }

	// Update
	void Update();

	// Draw
	void DrawAll(GLuint defFrameBuffer);
	void DrawFinalPass();

	void SetCanvasSize(int canvasWidth, int canvasHeight);

	void RegisterObject(IDrawable *object);
	void UnregisterObject(IDrawable *object);

	Camera* GetCamera(){ return m_camera; }

	void SetRenderType(int type){ m_type = type; }

	void SetDebugModeStatus(bool debugModeStatus){ m_debugModeStatus = debugModeStatus; }

	void ToggleChoppy()
	{
		if (m_waterIfft)
		{
			m_waterIfft->ToogleChoppy();
		}
	}

	void SetIFFT_L(int newVal){ m_waterIfft->SetL(newVal); }
	WaterIFFT* GetWaterIFFT(){ return m_waterIfft; }

private:

	QOpenGLContext *m_pGLContext = nullptr;
	QOpenGLFunctions_4_5_Core* f = nullptr;

	Camera *m_camera = nullptr;

	// transforms
	glm::mat4 m_mvp;
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;

	std::vector<float> m_waterParams;
	std::vector<float> m_wavesNormalMapData;

	float m_time;

	int m_canvasWidth;
	int m_canvasHeight;

	std::vector<IDrawable*> m_objects;

	GLuint m_fbo[3];
	GLuint m_colorTex[3][2];
	GLuint m_depthTex[3];

	GLuint m_defaultFramebuffer;

	ScreenQuad* m_screenQuad;
	ScreenQuad* m_screenQuad2;
	ScreenQuad* m_screenQuad3;
	ScreenQuad* m_screenQuad4;
	ScreenQuad* m_screenQuad5;

	std::vector<Cube*> m_cubes;

	Terrain* m_terrain;
	Water* m_water;
	WaterGerstner* m_waterGerstner;
	WaterIFFT* m_waterIfft;
	WorldGizmo* m_gizmo;

	Skybox* m_skybox;

	int m_type = 0;
	bool m_debugModeStatus = false;


	QTime m_qtime;
	int m_lastTime = 0;
	int m_dt = 0;
};


#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <random>
#include <functional>

#include <QOpenGLWidget>
#include <QOpenGLVersionProfile>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QShortcut>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "GerstnelParamManager.h"

#include "Renderer.h"

struct waterParams
{
	float Q;
	float S;
	float W;
	float L;
	float A;
	float DX;
	float DY;
};

class Rand_double
{
public:
	Rand_double(double low, double high)
		:r(std::bind(std::uniform_real_distribution<>(low, high), std::default_random_engine())) {}

	double operator()() { return r(); }

private:
	std::function<double()> r;
};

class OpenGLCanvas : public QOpenGLWidget//, protected QOpenGLFunctions_4_5_Core
{
public:
	OpenGLCanvas(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~OpenGLCanvas();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

protected:
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;

	void InitAll();

	void CreateWaveEditingWidget();

	// Init
	void InitGeometry();
	void InitBuffers();
	void InitQtConnections();
	void InitTextures();
	void InitWaterParams();

	void UpdateParamsWidgetsValues();

	// Saving
	void SaveCameraParams();

	// Rendering
	//void DrawAxis(glm::mat4x4 &mvp);
	//void DrawWater(glm::mat4x4 &mvp);
	//void DrawFlowWater(glm::mat4x4 &mvp);
	//void DrawSkybox(glm::mat4x4 &mvp);
	//void DrawLightCube(glm::mat4x4 &mvp);
	//void DrawFinalPass();
	//void DrawScenePortalRectangle();

	float GetNextRand();

private:
	float t = 0;
	GLfloat m_time = 0.0f;
	int m_nrFloats;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_colorsBufferId;
	GLuint m_skyboxVBO;
	GLuint m_lightPosVbo;
	GLuint m_lightTextureCoordsVbo;
	GLuint m_lightColorVbo;
	GLuint vbo_fbo_vertices;
	GLuint vbo_fbo_tex_vertices;

	GLuint m_colorTex[2];
	GLuint m_depthTex;
	GLuint m_fbo;

	// Events control variables
	bool m_mousePressed = false;
	bool m_isWireframeMode = false;

	// transforms
	glm::mat4 m_mvp;
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;

	// vbo data
	std::vector<GLfloat> m_patch_vert;
	std::vector<GLfloat> m_skyboxVertices;
	std::vector<GLfloat> m_lightCubeVertices;
	std::vector<GLfloat> m_lightTexCoord;
	GLfloat m_cubeSize;
	GLfloat m_lightCubeSize;

	// Shaders
	GLuint m_axisShader;
	GLuint m_terrainShader;
	GLuint m_waterflowShader;
	GLuint m_skyboxShader;
	GLuint m_filterShader;
	GLuint m_lightCubeShader;

	// Textures
	QOpenGLTexture* m_texture;
	QOpenGLTexture* m_waterColor;
	QOpenGLTexture* m_waterNormal;
	QOpenGLTexture* m_skyboxTex;
	//QOpenGLFramebufferObject* m_frameBufferTexture;

	QShortcut *m_saveCameraLocation;

	waterParams m_waterParams;
	std::vector<float> m_wavesGeometricData;
	std::vector<QDoubleSpinBox*> m_wavesGeometricDataWidgets;

	Rand_double rd{0.0, 1.0};

	GerstnelParamManager m_gerstnelParamManager;

	QLineEdit *m_configNameLineEdit = nullptr;
	QComboBox *m_comboConfigNames = nullptr;

	bool m_debugModeView = false;
	bool m_lightModeView = false;

	int targetCnt = 0;

	Renderer* m_renderer;

	QOpenGLContext* m_pGLContext = nullptr;
	QOpenGLFunctions_4_5_Core* f = nullptr;

	int m_cameraMoveFlags = 0;
};
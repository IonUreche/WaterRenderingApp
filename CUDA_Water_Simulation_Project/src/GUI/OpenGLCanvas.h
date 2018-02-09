#pragma once
#include <QOpenGLWidget>
#include <QOpenGLVersionProfile>
#include <QOpenGLFunctions_4_5_Core>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QShortcut>

#include "Camera.h"

class OpenGLCanvas : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
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

	void read_shader_src(const char *fname, std::vector<char> &buffer);
	GLuint load_and_compile_shader(const char *fname, GLenum shaderType);
	GLuint create_program(const char *path_vert_shader, const char *path_frag_shader,
		const char *path_tesc_shader, const char *path_tese_shader);

	// Init
	void InitShaders();
	void InitGeometry();
	void InitBuffers();
	void InitCamera(); 
	void InitQtConnections();
	void InitTextures();
	void InitLighting();

	// Saving
	void SaveCameraParams();

	// Rendering
	void DrawAxis(glm::mat4x4 &mvp);
	void DrawWater(glm::mat4x4 &mvp);
	void DrawSkybox(glm::mat4x4 &mvp);
	void DrawLightCube(glm::mat4x4 &mvp);

private:
	float t = 0;
	GLfloat m_time = 0.0f;
	int m_nrFloats;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_colorsBufferId;
	GLuint m_skyboxVBO;
	GLuint m_lightPosVbo;
	GLuint m_lightColorVbo;

	// Events control variables
	bool m_mousePressed = false;
	bool m_isWireframeMode = false;

	// Camera stuff
	Camera camera;

	// transforms
	glm::mat4 m_mvp;
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;

	// vbo data
	std::vector<GLfloat> m_coordinateAxisColors;
	std::vector<GLfloat> m_axisCoords;
	std::vector<GLfloat> m_patch_vert;
	std::vector<GLfloat> m_skyboxVertices;
	std::vector<GLfloat> m_lightCubeVertices;
	std::vector<GLfloat> m_lightColorVertices;
	GLfloat m_cubeSize;
	GLfloat m_lightCubeSize;

	// Shaders
	GLuint m_axisShader;
	GLuint m_terrainShader;
	GLuint m_skyboxShader;

	// Textures
	QOpenGLTexture *m_texture;
	QOpenGLTexture *m_waterColor;
	QOpenGLTexture *m_waterNormal;
	//std::vector<QOpenGLTexture*> m_skyboxFaces;
	QOpenGLTexture *m_skyboxTex;

	QShortcut *m_saveCameraLocation;
};
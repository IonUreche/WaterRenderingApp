﻿#include "OpenGLCanvas.h"
#include <QOpenGLFunctions_4_5_Core>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <fstream>

#include <QTimer>
#include <QKeySequence>

#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 5

/////////////////////////////////////////////////////////////////////////////////////
OpenGLCanvas::OpenGLCanvas(QWidget *parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent, f)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}
/////////////////////////////////////////////////////////////////////////////////////
OpenGLCanvas::~OpenGLCanvas()
{

}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitAll()
{
	InitShaders();
	InitLighting();
	InitGeometry();
	InitBuffers();
	InitCamera();
	InitQtConnections();
	InitTextures();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::initializeGL()
{
	initializeOpenGLFunctions();
	InitAll();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitTextures()
{
	GLint heightMapLoc = glGetUniformLocation(m_axisShader, "tex_displacement");
	glUniform1i(heightMapLoc, 0);
	// Prepare texture
	m_texture = new QOpenGLTexture(QImage("src/Resources/disp_map.jpg").mirrored());
	m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
	m_texture->bind(0);

	m_waterColor = new QOpenGLTexture(QImage("src/Resources/Water/water11.jpg").mirrored());
	GLint waterColorMapLoc = glGetUniformLocation(m_axisShader, "waterColorMap");
	glUniform1i(waterColorMapLoc, 1);
	// Prepare texture
	m_waterColor->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_waterColor->setMagnificationFilter(QOpenGLTexture::Linear);
	m_waterColor->bind(1);

	m_waterNormal = new QOpenGLTexture(QImage("src/Resources/Water/Water0Normal.jpg").mirrored());
	GLint waterNormalMapLoc = glGetUniformLocation(m_axisShader, "waterNormalMap");
	glUniform1i(waterNormalMapLoc, 2);
	// Prepare texture
	m_waterNormal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_waterNormal->setMagnificationFilter(QOpenGLTexture::Linear);
	m_waterNormal->bind(2);

	GLint skyboxTexLoc = glGetUniformLocation(m_skyboxShader, "skybox");
	glUniform1i(skyboxTexLoc, 0);

	for (int i = 0; i < 6; ++i)
	{
		QString s = "src/Resources/Skyboxes/12/sky12_";
		QOpenGLTexture::CubeMapFace face;
		switch (i)
		{
			case 0: s += "RT"; face = QOpenGLTexture::CubeMapPositiveX;  break;
			case 1: s += "LF"; face = QOpenGLTexture::CubeMapNegativeX;  break;
			case 2: s += "UP"; face = QOpenGLTexture::CubeMapNegativeY; break;
			case 3: s += "DN"; face = QOpenGLTexture::CubeMapPositiveY; break;
			case 4: s += "BK"; face = QOpenGLTexture::CubeMapPositiveZ;  break;
			case 5: s += "FR"; face = QOpenGLTexture::CubeMapNegativeZ; break;
		}
		s += ".jpg";

		const QImage img = QImage(s).mirrored().convertToFormat(QImage::Format_RGBA8888);
		if (i == 0)
		{
			m_skyboxTex = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
			m_skyboxTex->create();
			m_skyboxTex->setSize(img.width(), img.height(), img.depth());
			m_skyboxTex->setFormat(QOpenGLTexture::RGBA8_UNorm);
			m_skyboxTex->allocateStorage();
		}

		m_skyboxTex->setData(0, 0, face,
			QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
			(const void*)img.constBits(), 0);
	}
	m_skyboxTex->setWrapMode(QOpenGLTexture::ClampToEdge);
	m_skyboxTex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_skyboxTex->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_skyboxTex->generateMipMaps();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitBuffers()
{
	glEnable(GL_DEPTH_TEST);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferData(GL_ARRAY_BUFFER, m_axisCoords.size() * sizeof(GLfloat), m_axisCoords.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_colorsBufferId);
	//glBindBuffer(GL_ARRAY_BUFFER, m_colorsBufferId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_coordinateAxisColors.size(), m_coordinateAxisColors.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_skyboxVBO);

	glGenBuffers(1, &m_lightPosVbo);
	glGenBuffers(1, &m_lightColorVbo);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitQtConnections()
{
	QTimer *renderTimer = new QTimer(this);
	QObject::connect(renderTimer, SIGNAL(timeout()), this, SLOT(update()));
	renderTimer->start(16);

	m_saveCameraLocation = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Open")), this);
	QObject::connect(m_saveCameraLocation, &QShortcut::activated, this, &OpenGLCanvas::SaveCameraParams);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitLighting()
{
	m_lightColor = glm::vec3(231.0f, 212.0f, 105.0f);
	m_lightPosition = glm::vec3(50.0f, 30.0f, 50.0f);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::SaveCameraParams()
{
	std::ofstream fout("src/Configs/CameraParams.txt");
	fout << camera.camera_position.x << ' ' << camera.camera_position.y << ' ' << camera.camera_position.z << '\n';
	fout << camera.camera_look_at.x << ' ' << camera.camera_look_at.y << ' ' << camera.camera_look_at.z << '\n';
	fout << camera.camera_up.x << ' ' << camera.camera_up.y << ' ' << camera.camera_up.z << '\n';
	fout << camera.camera_pitch << ' ' << camera.camera_heading << ' ' << camera.camera_scale << '\n';

	fout.close();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitCamera()
{
	// Camera initialization
	//glm::vec3 Obs = glm::vec3(-50, 60.0f, -50.0f);
	//glm::vec3 RefPct = glm::vec3(0.0f, 0.0f, 0.0f);

	std::ifstream fin("src/Configs/CameraParams.txt");
	if(!fin.is_open()) assert(false);
	float x, y, z;
	fin >> x >> y >> z;
	glm::vec3 Obs = glm::vec3(x, y, z);
	fin >> x >> y >> z;
	glm::vec3 RefPct = glm::vec3(x, y, z);
	fin >> x >> y >> z;
	glm::vec3 cameraUp = glm::vec3(x, y, z);
	fin >> x >> y >> z;
	camera.camera_pitch = x;
	camera.camera_heading = y;
	camera.camera_scale = z;
	fin.close();

	int canvasWidth = this->width();
	int canvasHeight = this->height();
	float width = canvasWidth, height = canvasHeight, znear = 0.1f, zfar = 20000.0f, fov = 45;

	camera.SetProgramId(m_axisShader);
	camera.SetPosition(Obs);
	camera.SetLookAt(RefPct);
	camera.camera_up = cameraUp;

	camera.SetFOV(fov);
	camera.aspect = GLfloat(width) / GLfloat(height);
	camera.near_clip = znear;
	camera.far_clip = zfar;
	camera.move_camera = true;
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::paintGL()
{
	camera.Update();
	m_mvp = camera.projection * camera.view;
	m_time += 0.1;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Vao
	glBindVertexArray(m_vao);

	DrawSkybox(m_mvp);
	DrawAxis(m_mvp);
	DrawWater(m_mvp);
	DrawLightCube(m_mvp);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::DrawAxis(glm::mat4x4 &mvp)
{
	// Shader
	glUseProgram(m_axisShader);

	// Uniforms
	GLint uniMvp = glGetUniformLocation(m_axisShader, "mvp");
	glUniformMatrix4fv(uniMvp, 1, GL_FALSE, glm::value_ptr(m_mvp));

	// position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_axisCoords.size() * sizeof(GLfloat), m_axisCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// color
	glBindBuffer(GL_ARRAY_BUFFER, m_colorsBufferId);
	glBufferData(GL_ARRAY_BUFFER, m_coordinateAxisColors.size() * sizeof(GLfloat), m_coordinateAxisColors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw 
	glDrawArrays(GL_LINES, 0, m_axisCoords.size());
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::DrawLightCube(glm::mat4x4 &mvp)
{
	glUseProgram(m_axisShader);

	glBindBuffer(GL_ARRAY_BUFFER, m_lightPosVbo);
	glBufferData(GL_ARRAY_BUFFER, m_lightCubeVertices.size() * sizeof(GLfloat), m_lightCubeVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_lightColorVbo);
	glBufferData(GL_ARRAY_BUFFER, m_lightColorVertices.size() * sizeof(GLfloat), m_lightColorVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	GLuint mvpLocation = glGetUniformLocation(m_skyboxShader, "mvp");
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// draw
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::DrawWater(glm::mat4x4 &mvp)
{
	// DRAW TERRAIN PATCHES
	glUseProgram(m_terrainShader);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_patch_vert.size() * sizeof(GLfloat), m_patch_vert.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLuint mvpLocation = glGetUniformLocation(m_terrainShader, "mvp");
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(m_mvp));

	GLuint timeLocation = glGetUniformLocation(m_terrainShader, "time");
	glUniform1f(timeLocation, m_time);
	//
	//glEnable(GL_TEXTURE0);
	//glEnable(GL_TEXTURE1);
	//glEnable(GL_TEXTURE2);
	m_texture->bind(0);
	m_waterColor->bind(1);
	m_waterNormal->bind(2);
	////m_program->setUniformValue("colorTexture", 0);
	//
	if(m_isWireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::DrawSkybox(glm::mat4x4 &mvp)
{
	glDepthMask(GL_FALSE);
	glUseProgram(m_skyboxShader);

	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, m_skyboxVertices.size() * sizeof(GLfloat), m_skyboxVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLuint mvpLocation = glGetUniformLocation(m_skyboxShader, "mvp");
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

	glEnable(GL_TEXTURE0);
	m_skyboxTex->bind(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// draw
	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
	
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	//glUniform1i(glGetUniformLocation(m_ShaderProgramId, "skybox"), 0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::resizeGL(int w, int h)
{
	//int side = qMin(width, height);
	//glViewport((width - side) / 2, (height - side) / 2, side, side);
}
///////////////////////// INPUT HANDLING CODE ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::mousePressEvent(QMouseEvent *event)
{
	m_mousePressed = true;
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::mouseMoveEvent(QMouseEvent *event)
{
	if (m_mousePressed)
	{
		camera.Move2D(event->pos().x(), event->pos().y());
	}
	else
	{
		camera.mouse_position = glm::vec3(event->pos().x(), event->pos().y(), 0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::mouseReleaseEvent(QMouseEvent *event)
{
	m_mousePressed = false;
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_W:
		camera.Move(FORWARD);
		break;
	case Qt::Key_A:
		camera.Move(LEFT);
		break;
	case Qt::Key_S:
		camera.Move(BACK);
		break;
	case Qt::Key_D:
		camera.Move(RIGHT);
		break;
	case Qt::Key_Q:
		camera.Move(DOWN);
		break;
	case Qt::Key_E:
		camera.Move(UP);
		break;
	case Qt::Key_T:
		m_isWireframeMode ^= 1;
		break;
		}
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::keyReleaseEvent(QKeyEvent *event)
{
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitGeometry()
{
	// axis vertices
	m_axisCoords =
	{
		0.0f, 0.0f, 0.0f,
		10.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 10.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 10.0f
	};
	// axis colors
	m_coordinateAxisColors =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	// init patch vertices
	m_patch_vert =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	m_cubeSize = 5000;
	m_skyboxVertices = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	m_lightCubeSize = 3;
	m_lightCubeVertices.resize(36 * 3);
	for (int i = 0; i < 36; ++i)
	{
		m_lightCubeVertices[i * 3] = m_lightPosition.x + m_skyboxVertices[i * 3] * m_lightCubeSize;
		m_lightCubeVertices[i * 3 + 1] = m_lightPosition.y + m_skyboxVertices[i * 3 + 1] * m_lightCubeSize;
		m_lightCubeVertices[i * 3 + 2] = m_lightPosition.z + m_skyboxVertices[i * 3 + 2] * m_lightCubeSize;
	}

	m_lightColorVertices.resize(36 * 3);
	for (int i = 0; i < 36; ++i)
	{
		m_lightColorVertices[i * 3] = m_lightColor.x / 255.0f;
		m_lightColorVertices[i * 3 + 1] = m_lightColor.y / 255.0f;
		m_lightColorVertices[i * 3 + 2] = m_lightColor.z / 255.0f;
	}

	for (int i = 0; i < 36 * 3; ++i)
		m_skyboxVertices[i] = m_skyboxVertices[i] * m_cubeSize;
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitShaders()
{
	m_axisShader = create_program("src/Shaders/basic3D.vert", "src/Shaders/basic3D.frag", "", "");
	m_terrainShader = create_program("src/Shaders/terrain.vert", "src/Shaders/terrain.frag",
		"src/Shaders/terrain.tesc", "src/Shaders/terrain.tese");
	m_skyboxShader = create_program("src/Shaders/skybox.vert", "src/Shaders/skybox.frag", "", "");
}
/////////////////////////////////////////////////////////////////////////////////////
GLuint OpenGLCanvas::create_program(const char *path_vert_shader, const char *path_frag_shader,
	const char *path_tesc_shader, const char *path_tese_shader) {

	GLuint shaderProgram = glCreateProgram();

	// Load and compile the vertex and fragment shaders
	if (strcmp(path_vert_shader, "") != 0)
	{
		GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
		glAttachShader(shaderProgram, vertexShader);
		glDeleteShader(vertexShader);
	}
	else
	{
		assert(false);
	}

	if (strcmp(path_frag_shader, "") != 0)
	{
		GLuint fragmentShader = load_and_compile_shader(path_frag_shader, GL_FRAGMENT_SHADER);
		glAttachShader(shaderProgram, fragmentShader);
		glDeleteShader(fragmentShader);
	}
	else
	{
		assert(false);
	}

	if (strcmp(path_tesc_shader, "") != 0)
	{
		GLuint tessControlShader = load_and_compile_shader(path_tesc_shader, GL_TESS_CONTROL_SHADER);
		glAttachShader(shaderProgram, tessControlShader);
		glDeleteShader(tessControlShader);
	}

	if (strcmp(path_tese_shader, "") != 0)
	{
		GLuint tessEvalShader = load_and_compile_shader(path_tese_shader, GL_TESS_EVALUATION_SHADER);
		glAttachShader(shaderProgram, tessEvalShader);
		glDeleteShader(tessEvalShader);
	}

	// Link and use the program
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	return shaderProgram;
}
/////////////////////////////////////////////////////////////////////////////////////
GLuint OpenGLCanvas::load_and_compile_shader(const char *fname, GLenum shaderType) {
	// Load a shader from an external file
	std::vector<char> buffer;
	read_shader_src(fname, buffer);
	buffer.push_back('\0');
	const char *src = &buffer[0];

	// Compile the shader
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	// Check the result of the compilation
	GLint test;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if (!test) {
		std::cerr << "Shader compilation failed with this message:" << std::endl;
		std::vector<char> compilation_log(512);
		glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
		std::cerr << &compilation_log[0] << std::endl;
		exit(-1);
		return -1;
	}
	return shader;
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::read_shader_src(const char *fname, std::vector<char> &buffer)
{
	std::ifstream f(fname);
	std::string str;
	if (f)
	{
		f.seekg(0, std::ios::end);
		str.reserve(f.tellg());
		f.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());

		// IMPROVE: AVOID copy from string to vector<char>
		buffer.assign(str.begin(), str.end());
	}
	else
	{
		assert(false);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
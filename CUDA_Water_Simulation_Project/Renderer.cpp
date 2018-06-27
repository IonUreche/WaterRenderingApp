#include "Renderer.h"
#include "ShaderManager.h"

#include <fstream>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"

#include "TextureManager.h"


Renderer::Renderer(QOpenGLContext *glContext, GLuint defaultFrameBuffer)
{
	m_qtime.start();
	m_lastTime = m_qtime.elapsed();

	m_type = 2;
	m_defaultFramebuffer = defaultFrameBuffer;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();

	m_camera = new Camera();
	ShaderManager* inst = ShaderManager::GetInstance();
	inst->SetGLContext(glContext);
	m_time = 0;
	auto inst2 = TextureManager::GetInstance();
	inst2->SetGLContext(glContext);
	inst2->Init();

	m_qtime.start();
}
//////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitAll()
{
	InitShaders();
	InitCamera();
	InitLighting();
	InitFrameBuffer();
	InitObjects();
	InitBuffers();
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitShaders()
{
	ShaderManager* inst = ShaderManager::GetInstance();
	inst->InitShaders();
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitCamera()
{
	std::ifstream fin("src/Configs/CameraParams.txt");
	if (!fin.is_open()) assert(false);
	float x, y, z;
	fin >> x >> y >> z;
	glm::vec3 Obs = glm::vec3(x, y, z);
	fin >> x >> y >> z;
	glm::vec3 RefPct = glm::vec3(x, y, z);
	fin >> x >> y >> z;
	glm::vec3 cameraUp = glm::vec3(x, y, z);
	fin >> x >> y >> z;
	m_camera->camera_pitch = x;
	m_camera->camera_heading = y;
	//m_camera->camera_scale = z;
	fin.close();

	float width = m_canvasWidth, height = m_canvasHeight, znear = 1.0f, zfar = 10000.0f, fov = 45;

	//camera.SetProgramId(m_axisShader);
	m_camera->SetPosition(Obs);
	m_camera->SetLookAt(RefPct);
	m_camera->camera_up = cameraUp;

	m_camera->SetFOV(fov);
	m_camera->aspect = GLfloat(width) / GLfloat(height);
	m_camera->near_clip = znear;
	m_camera->far_clip = zfar;
	m_camera->move_camera = true;
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitLighting()
{
	//m_lightColor = glm::vec3(231.0f, 212.0f, 105.0f);
	//m_lightPosition = glm::vec3(50.0f, 30.0f, 50.0f);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitFrameBuffer()
{
	f->glEnable(GL_DEPTH_TEST);
	// generate a framebuffer 
	f->glGenFramebuffers(3, m_fbo);
	// bind it as the target for rendering commands
	for (int fo = 0; fo < 3; ++fo)
	{
		f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo[fo]);

		int width = 1024, height = 768;

		f->glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
		f->glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
		f->glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 4);

		//GLuint m_fb;
		//glGenRenderbuffers(1, &m_fb);
		//glBindRenderbuffer(GL_RENDERBUFFER, m_fb);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 768);
		//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFERf, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fb);

		//GLuint m_colorTex[2];
		// create n RGBA color textures
		int ColorAttachmentsN = 2;
		f->glGenTextures(ColorAttachmentsN, m_colorTex[fo]);

		for (int i = 0; i < ColorAttachmentsN; ++i) {
			f->glBindTexture(GL_TEXTURE_2D, m_colorTex[fo][i]);
			f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
				width, height,
				0, GL_RGBA, GL_UNSIGNED_BYTE,
				NULL);
			
			f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			f->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTex[fo][i], 0);
		}

		// create a depth texture
		f->glGenTextures(1, m_depthTex + fo);
		f->glBindTexture(GL_TEXTURE_2D, m_depthTex[fo]);

		f->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height,
			0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);
		f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// create the framebuffer object
		//f->glGenFramebuffers(1, &m_fbo);
		//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		//// attach colors
		//for (int i = 0; i < ColorAttachmentsN; ++i) {
		//	f->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
		//		GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTex[fo][i], 0);
		//}
		// attach color
		f->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex[fo], 0);

		GLenum e = f->glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (e != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("There is a problem with the m_fbo\n");
			char ch;
			std::cin >> ch;
			exit(-1);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitBuffers()
{
	for (auto obj : m_objects)
	{
		obj->InitGeometry();
		obj->InitBuffers();
	}
	
	//glEnable(GL_DEPTH_TEST);
	//glGenVertexArrays(1, &m_vao);
	//glBindVertexArray(m_vao);
	//glGenBuffers(1, &m_vbo);
	//glGenBuffers(1, &m_colorsBufferId);
	//glGenBuffers(1, &m_skyboxVBO);
	//glGenBuffers(1, &m_lightPosVbo);
	//glGenBuffers(1, &m_lightTextureCoordsVbo);
	//glGenBuffers(1, &vbo_fbo_vertices);
	//glGenBuffers(1, &vbo_fbo_tex_vertices);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitObjects()
{
	m_terrain = new Terrain(m_pGLContext, m_fbo[0]);
	m_water = new Water(m_pGLContext, m_fbo[0]);
	m_gizmo = new WorldGizmo(m_pGLContext, m_fbo[0]);

	m_waterGerstner = new WaterGerstner(m_pGLContext, m_fbo[0]);
	m_waterIfft = new WaterIFFT(m_pGLContext, m_fbo[0]);

	RegisterObject(m_terrain);
	RegisterObject(m_water);
	RegisterObject(m_gizmo);
	RegisterObject(m_waterGerstner);
	RegisterObject(m_waterIfft);

	m_screenQuad = new ScreenQuad(m_pGLContext);
	m_screenQuad->SetRenderTarget(m_defaultFramebuffer);
	m_screenQuad->SetColorAttachment(m_colorTex[2][0]);
	m_screenQuad->InitBuffers();

	m_screenQuad2 = new ScreenQuad(m_pGLContext);
	m_screenQuad2->SetRenderTarget(m_defaultFramebuffer);
	m_screenQuad2->SetType(2);
	m_screenQuad2->SetFlipV(1);
	m_screenQuad2->SetColorAttachment(m_colorTex[0][0]);
	m_screenQuad2->InitBuffers();

	m_screenQuad3 = new ScreenQuad(m_pGLContext);
	m_screenQuad3->SetRenderTarget(m_defaultFramebuffer);
	m_screenQuad3->SetType(2);
	m_screenQuad3->SetFlipV(1);
	m_screenQuad3->SetColorAttachment(m_colorTex[1][0]);
	m_screenQuad3->InitBuffers();

	m_screenQuad4 = new ScreenQuad(m_pGLContext);
	m_screenQuad4->SetRenderTarget(m_defaultFramebuffer);
	m_screenQuad4->SetType(2);
	m_screenQuad4->SetFlipV(1);
	m_screenQuad4->SetColorAttachment(m_colorTex[0][1]/*m_depthTex[0]*/);
	m_screenQuad4->InitBuffers();
	//RegisterObject(_screenQuad);

	m_screenQuad5 = new ScreenQuad(m_pGLContext);
	m_screenQuad5->SetRenderTarget(m_defaultFramebuffer);
	m_screenQuad5->SetType(2);
	m_screenQuad5->SetFlipV(1);
	m_screenQuad5->SetColorAttachment(m_colorTex[0][1]/*m_depthTex[0]*/);
	m_screenQuad5->InitBuffers();

	int n_cubes = 3;

	float cubes_pos[] = {
		54.0f, 20.0f, 85.f,
		60.0f, 13.0f, 47.0f,
		80.0f, -2.0f, 75.0f
	};

	float cubes_size[] = {
		3.0f, 2.0f, 1.0f
	};
	
	for (int i = 0; i < n_cubes; ++i)
	{
		m_cubes.push_back(new Cube(m_pGLContext, m_fbo[0]));
		m_cubes[i]->SetCubeSize(cubes_size[i]);
		m_cubes[i]->SetPosition(glm::vec3(cubes_pos[i*3], cubes_pos[i * 3 + 1], cubes_pos[i * 3 + 2]));
		m_cubes[i]->InitGeometry();
		m_cubes[i]->InitBuffers();
	}

	m_skybox = new Skybox(m_pGLContext, m_fbo[0]);
	m_skybox->SetSize(1000);
	m_skybox->InitGeometry();
	m_skybox->InitBuffers();
}
//////////////////////////////////////////////////////////////////////////
void Renderer::Update()
{
	m_mvp = m_camera->projection * m_camera->view;
	m_time += 0.08;
}
//////////////////////////////////////////////////////////////////////////
void Renderer::DrawAll(GLuint defFrameBuffer)
{
	int aux = m_qtime.elapsed();
	m_dt = aux - m_lastTime;
	m_lastTime = aux;

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT };

	if (m_type == 0)
	{
		// FBO # 0 (Refraction)
		f->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]);
		f->glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
		f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		f->glEnable(GL_DEPTH_TEST);

		
		f->glDrawBuffers(3, attachments);
		f->glEnable(GL_CLIP_DISTANCE0);

		m_terrain->SetPlane(glm::vec4(0, -1, 0, 2));
		for (int i = 0; i < m_cubes.size(); ++i)
		{
			if (m_cubes[i]->GetPosition().y < 0)
			{
				m_cubes[i]->Draw(m_mvp, m_camera->camera_position);
			}
		}
	
		m_terrain->Draw(m_mvp, m_camera->camera_position);
	}

	if(m_type == 0 || m_type == 1)
	{
		// FBO #1 (reflection)
		f->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[1]);
		f->glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
		f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		f->glDrawBuffers(3, attachments);

		auto cmps = m_camera->camera_position;

		auto cmv1 = m_camera->view;
		cmv1[1][0] *= -1;
		cmv1[1][1] *= -1;
		cmv1[1][2] *= -1;
		auto mvp1 = m_camera->projection * cmv1;

		cmps.y = -cmps.y;

		m_skybox->Draw(mvp1, m_camera->camera_position);
		if (m_type == 0)
		{
			m_terrain->SetPlane(glm::vec4(0, 1, 0, 0));
			m_terrain->Draw(mvp1, cmps);
		}
		//m_gizmo->Draw(mvp1, m_camera->camera_position);
		if (m_type == 0)
		{
			for (int i = 0; i < m_cubes.size(); ++i)
			{
				if (m_cubes[i]->GetPosition().y >= 0)
				{
					m_cubes[i]->Draw(mvp1, cmps);
				}
			}
		}
	}

	if (m_type == 0 || m_type == 1 || m_type == 2)
	{
		// FBO #2 (The whole scene)
		f->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[2]);
		f->glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		f->glDrawBuffers(3, attachments);

		m_skybox->Draw(m_mvp, m_camera->camera_position);
		m_gizmo->Draw(m_mvp, m_camera->camera_position);

		if(m_type == 0)
		{
			m_terrain->SetPlane(glm::vec4(0, 1, 0, 11111));
			m_water->SetPlane(glm::vec4(0, -1, 0, 11111));
			m_terrain->Draw(m_mvp, m_camera->camera_position);
		
			//m_gizmo->Draw(m_mvp, m_camera->camera_position);
			for (int i = 0; i < m_cubes.size(); ++i)
			{
				m_cubes[i]->Draw(m_mvp, m_camera->camera_position);
			}

			f->glEnable(GL_BLEND);
			f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			m_water->SetWaterTextures(m_colorTex[0][0], m_colorTex[1][0]);
			m_water->SetDepthTexture(m_depthTex[0]);
			m_water->m_view = m_camera->view;
			m_water->Draw(m_mvp, m_camera->camera_position);
			f->glDisable(GL_BLEND);
		}
		else
		if (m_type == 1)
		{
			//m_gizmo->Draw(m_mvp, m_camera->camera_position);

			m_waterGerstner->SetWaterTextures(m_colorTex[0][0], m_colorTex[1][0]);
			m_waterGerstner->SetDepthTexture(m_depthTex[0]);
			//m_waterGerstner->SetSkyboxCubeMap();
			m_waterGerstner->SetProjectionMatrix(m_camera->projection);
			m_waterGerstner->SetGerstnerParams(m_waterParams);
			m_waterGerstner->SetGerstnernormalsParams(m_waterParams);
			m_waterGerstner->Draw(m_mvp, m_camera->camera_position, m_debugModeStatus);
		}
		else
		if (m_type == 2)
		{
			m_waterIfft->SetDt(m_dt * 0.001);
			m_waterIfft->Draw(m_mvp, m_camera->camera_position, m_debugModeStatus);
			m_screenQuad2->SetColorAttachment(m_waterIfft->GetTildehktX());
			m_screenQuad3->SetColorAttachment(m_waterIfft->GetTildehktY());
			m_screenQuad4->SetColorAttachment(m_waterIfft->GetTildehktZ());
			m_screenQuad5->SetColorAttachment(m_waterIfft->GetTwiddleIndices());
		}
	}
	f->glDisable(GL_DEPTH_TEST);
	f->glDisable(GL_CLIP_DISTANCE0);

	f->glBindFramebuffer(GL_FRAMEBUFFER, defFrameBuffer);
	//f->glViewport(0, 0, width, height);
	f->glClearColor(0.4f, 0.8f, 0.4f, 1.0f);
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// The scene with post-processing effect & all the GUI elements
	std::vector<GLfloat> screenVert = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
	m_screenQuad->SetFBOVertices(screenVert);
	m_screenQuad->Draw(m_mvp, m_camera->camera_position);

	std::vector<GLfloat> leftGUIVert = { -0.9f, 0.9f, -0.5f, 0.9f, -0.9f, 0.5f, -0.5f, 0.5f };
	//std::vector<GLfloat> leftGUIVert = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
	m_screenQuad2->SetFBOVertices(leftGUIVert);
	m_screenQuad2->Draw(m_mvp, m_camera->camera_position);

	std::vector<GLfloat> rightGUIVert = { 0.5f, 0.9f, 0.9f, 0.9f, 0.5f, 0.5f, 0.9f, 0.5f };
	//std::vector<GLfloat> rightGUIVert = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
	//std::vector<GLfloat> rightGUIVert = { 0.6f, 0.9f, 0.9f, 0.9f, 0.6f, 0.5f, 0.9f, 0.5f };
	m_screenQuad3->SetFBOVertices(rightGUIVert);
	m_screenQuad3->Draw(m_mvp, m_camera->camera_position);

	//std::vector<GLfloat> rightBotGUIVert = { 0.5f, -0.5f, 0.9f, -0.5f, 0.5f, -0.9f, 0.9f, -0.9f };
	//std::vector<GLfloat> rightBotGUIVert = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<GLfloat> rightBotGUIVert = { 0.5f, -0.5f, 0.9f, -0.5f, 0.5f, -0.9f, 0.9f, -0.9f };
	m_screenQuad4->SetFBOVertices(rightBotGUIVert);
	m_screenQuad4->Draw(m_mvp, m_camera->camera_position);

	std::vector<GLfloat> leftBotGUIVert = { -0.9f, -0.5f, -0.5f, -0.5f, -0.9f, -0.9f, -0.5f, -0.9f };
	m_screenQuad5->SetFBOVertices(leftBotGUIVert);
	m_screenQuad5->Draw(m_mvp, m_camera->camera_position);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::DrawFinalPass()
{
//	GLuint timeLocation = glGetUniformLocation(m_filterShader, "time");
//	glUniform1f(timeLocation, m_time);
//
//	GLfloat fbo_vertices[] = {
//		-1.0f, -1.0f,
//		1.0f, -1.0f,
//		-1.0f, 1.0f,
//		1.0f, 1.0f,
//	};
//
//	GLfloat fbo_tex_vertices[] = {
//		0.0f, 0.0f,
//		1.0f, 0.0f,
//		0.0f, 1.0f,
//		1.0f, 1.0f,
//		};
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_tex_vertices);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_tex_vertices), fbo_tex_vertices, GL_STATIC_DRAW);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(1);
//
//	glEnable(GL_TEXTURE0);
//	GLuint sceneTextureLocation = glGetUniformLocation(m_filterShader, "renderedTexture");
//	glUniform1i(sceneTextureLocation, 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_colorTex[0]);
//
//
//	//if (m_isWireframeMode)
//	//{
//	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	//}
//	//else
//	//{
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	//}
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::RegisterObject(IDrawable *object)
{
	m_objects.push_back(object);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::UnregisterObject(IDrawable *object)
{
	auto it = std::find(m_objects.begin(), m_objects.end(), object);
	if (it != m_objects.end())
	{
		m_objects.erase(it);
	}
}
//////////////////////////////////////////////////////////////////////////
void Renderer::SetCanvasSize(int canvasWidth, int canvasHeight)
{
	m_canvasWidth = canvasWidth;
	m_canvasHeight = canvasHeight;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
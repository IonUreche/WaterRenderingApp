#include "OpenGLCanvas.h"
#include <QOpenGLFunctions_4_5_Core>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"

#include <iostream>
#include <fstream>

#include <QTimer>
#include <QKeySequence>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_5_Core>

#include "Camera.h"
#include "ShaderManager.h"

#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 5

/////////////////////////////////////////////////////////////////////////////////////
OpenGLCanvas::OpenGLCanvas(QWidget *parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent, f)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	//setSizeHi(QSize(1024, 768));
}
/////////////////////////////////////////////////////////////////////////////////////
OpenGLCanvas::~OpenGLCanvas()
{

}
float OpenGLCanvas::GetNextRand()
{
	return rd();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::CreateWaveEditingWidget()
{
	QWidget *wid = new QWidget(this);
	QHBoxLayout *hlay = new QHBoxLayout();
	hlay->setContentsMargins(1, 1, 1, 1);
	wid->setLayout(hlay);
	for (int i = 0; i < 4; ++i)
	{
		QWidget *vcontainer = new QWidget();
		QVBoxLayout *vlay = new QVBoxLayout();
		vlay->setContentsMargins(1, 1, 1, 1);
		vcontainer->setLayout(vlay);

		// create widgets
		// L Widget
		QWidget *w1 = new QWidget();
		QHBoxLayout *h1 = new QHBoxLayout();
		h1->setContentsMargins(1, 1, 1, 1);
		w1->setLayout(h1);
		QLabel *l1 = new QLabel();
		l1->setText("L: ");
		h1->addWidget(l1);
		QDoubleSpinBox *s1 = new QDoubleSpinBox();
		s1->setValue(m_wavesGeometricData[6*i]);
		m_wavesGeometricDataWidgets.push_back(s1);
		h1->addWidget(s1);
		QObject::connect(s1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
		{
			m_wavesGeometricData[6 * i] = newVal;
		});
		vlay->addWidget(w1);

		// Dx Widget
		m_wavesGeometricDataWidgets.push_back(new QDoubleSpinBox());

		// Dy Widget
		m_wavesGeometricDataWidgets.push_back(new QDoubleSpinBox());

		// Q Widget
		QWidget *w4 = new QWidget();
		QHBoxLayout *h4 = new QHBoxLayout();
		h4->setContentsMargins(1, 1, 1, 1);
		w4->setLayout(h4);
		QLabel *l4 = new QLabel();
		l4->setText("Q: ");
		h4->addWidget(l4);
		QDoubleSpinBox *s4 = new QDoubleSpinBox();
		s4->setValue(m_wavesGeometricData[6 * i + 3]);
		m_wavesGeometricDataWidgets.push_back(s4);
		h4->addWidget(s4);
		QObject::connect(s4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
		{
			m_wavesGeometricData[6 * i + 3] = newVal;
		});
		vlay->addWidget(w4);

		// S Widget
		QWidget *w5 = new QWidget();
		QHBoxLayout *h5 = new QHBoxLayout();
		h5->setContentsMargins(1, 1, 1, 1);
		w5->setLayout(h5);
		QLabel *l5 = new QLabel();
		l5->setText("S: ");
		h5->addWidget(l5);
		QDoubleSpinBox *s5 = new QDoubleSpinBox();
		s5->setValue(m_wavesGeometricData[6 * i + 4]);
		m_wavesGeometricDataWidgets.push_back(s5);
		h5->addWidget(s5);
		QObject::connect(s5, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
		{
			m_wavesGeometricData[6 * i + 4] = newVal;
		});
		vlay->addWidget(w5);

		// A Widget
		QWidget *w6 = new QWidget();
		QHBoxLayout *h6 = new QHBoxLayout();
		h6->setContentsMargins(1, 1, 1, 1);
		w6->setLayout(h6);
		QLabel *l6 = new QLabel();
		l6->setText("A: ");
		h6->addWidget(l6);
		QDoubleSpinBox *s6 = new QDoubleSpinBox();
		s6->setValue(m_wavesGeometricData[6 * i + 5]);
		m_wavesGeometricDataWidgets.push_back(s6);
		h6->addWidget(s6);
		QObject::connect(s6, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
		{
			m_wavesGeometricData[6 * i + 5] = newVal;
		});
		vlay->addWidget(w6);

		if (i == 0)
		{
			m_configNameLineEdit = new QLineEdit();
			vlay->addWidget(m_configNameLineEdit);
		}
		else if(i == 1)
		{
			m_comboConfigNames = new QComboBox();
			vlay->addWidget(m_comboConfigNames);

			auto names = m_gerstnelParamManager.GetConfigsNames();
			for (auto &n : names)
			{
				m_comboConfigNames->addItem(QString(n.c_str()));
			}

			QObject::connect(m_comboConfigNames, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
			{
				auto configFloats = m_gerstnelParamManager.GetConfigData(index);
				for (int i = 0; i < 24; ++i)
				{
					if(i % 6 != 1 && i % 6 != 2)
						m_wavesGeometricData[i] = configFloats[i];
				}
				m_wavesGeometricData = configFloats;
				UpdateParamsWidgetsValues();

				m_configNameLineEdit->setText(m_comboConfigNames->itemText(index));
			});
		}
		else if (i == 2)
		{
			// Add Save button only for first widget
			QPushButton *pb = new QPushButton();
			pb->setText("Save");
			QObject::connect(pb, &QPushButton::released, [this]()
			{
				if (m_wavesGeometricData.size() == 24
					&& m_configNameLineEdit)
				{
					bool newConfigCreated = m_gerstnelParamManager.AddConfig(m_configNameLineEdit->text().toLatin1().data(), m_wavesGeometricData.data());
					m_gerstnelParamManager.SaveToFile();
					if (newConfigCreated)
					{
						m_comboConfigNames->addItem(m_configNameLineEdit->text().toLatin1().data());
						m_comboConfigNames->setCurrentIndex(m_comboConfigNames->count() - 1);
					}
				}
			});

			vlay->addWidget(pb);
		}

		hlay->addWidget(vcontainer);
	}

	wid->hide();
}
void OpenGLCanvas::UpdateParamsWidgetsValues()
{
	for(int i = 0; i < 24; ++i)
		m_wavesGeometricDataWidgets[i]->setValue(m_wavesGeometricData[i]);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitAll()
{
	m_pGLContext = context();
	f = m_pGLContext->versionFunctions<QOpenGLFunctions_4_5_Core>();

	InitBuffers();

	m_renderer = new Renderer(context(), defaultFramebufferObject());
	m_renderer->SetCanvasSize(width(), height());
	m_renderer->InitAll();

	InitGeometry();
	
	InitQtConnections();
	InitTextures();
	InitWaterParams();

	CreateWaveEditingWidget();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::initializeGL()
{
	//initializeOpenGLFunctions();
	InitAll();
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitTextures()
{
	//GLint heightMapLoc = glGetUniformLocation(m_axisShader, "tex_displacement");
	//glUniform1i(heightMapLoc, 0);
	//
	//m_waterColor = new QOpenGLTexture(QImage("src/Resources/Water/water11.jpg").mirrored());
	//GLint waterColorMapLoc = glGetUniformLocation(m_axisShader, "waterColorMap");
	//glUniform1i(waterColorMapLoc, 1);
	//// Prepare texture
	//m_waterColor->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	//m_waterColor->setMagnificationFilter(QOpenGLTexture::Linear);
	//m_waterColor->bind(1);
	//
	//m_waterNormal = new QOpenGLTexture(QImage("src/Resources/Water/Water0Normal.jpg").mirrored());
	//GLint waterNormalMapLoc = glGetUniformLocation(m_axisShader, "waterNormalMap");
	//glUniform1i(waterNormalMapLoc, 2);
	//// Prepare texture
	//m_waterNormal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	//m_waterNormal->setMagnificationFilter(QOpenGLTexture::Linear);
	//m_waterNormal->bind(2);
	//
	//GLint skyboxTexLoc = glGetUniformLocation(m_skyboxShader, "skybox");
	//glUniform1i(skyboxTexLoc, 0);
	//
	//for (int i = 0; i < 6; ++i)
	//{
	//	QString s = "src/Resources/Skyboxes/12/test/sky12_";
	//	QOpenGLTexture::CubeMapFace face;
	//	switch (i)
	//	{
	//		case 0: s += "RT"; face = QOpenGLTexture::CubeMapPositiveX;  break;
	//		case 1: s += "LF"; face = QOpenGLTexture::CubeMapNegativeX;  break;
	//		case 2: s += "UP"; face = QOpenGLTexture::CubeMapNegativeY; break;
	//		case 3: s += "DN"; face = QOpenGLTexture::CubeMapPositiveY; break;
	//		case 4: s += "BK"; face = QOpenGLTexture::CubeMapPositiveZ;  break;
	//		case 5: s += "FR"; face = QOpenGLTexture::CubeMapNegativeZ; break;
	//	}
	//	s += ".jpg";
	//
	//	const QImage img = QImage(s).mirrored().convertToFormat(QImage::Format_RGBA8888);
	//	if (i == 0)
	//	{
	//		m_skyboxTex = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
	//		m_skyboxTex->create();
	//		m_skyboxTex->setSize(img.width(), img.height(), img.depth());
	//		m_skyboxTex->setFormat(QOpenGLTexture::RGBA8_UNorm);
	//		m_skyboxTex->allocateStorage();
	//	}
	//
	//	m_skyboxTex->setData(0, 0, face,
	//		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
	//		(const void*)img.constBits(), 0);
	//}
	//m_skyboxTex->setWrapMode(QOpenGLTexture::ClampToEdge);
	//m_skyboxTex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	//m_skyboxTex->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
	//m_skyboxTex->generateMipMaps();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitBuffers()
{
	f->glEnable(GL_DEPTH_TEST);
	f->glGenVertexArrays(1, &m_vao);
	f->glBindVertexArray(m_vao);
	return ;
	//glGenBuffers(1, &m_vbo);
	//glGenBuffers(1, &m_colorsBufferId);
	//glGenBuffers(1, &m_skyboxVBO);
	//glGenBuffers(1, &m_lightPosVbo);
	//glGenBuffers(1, &m_lightTextureCoordsVbo);
	//glGenBuffers(1, &vbo_fbo_vertices);
	//glGenBuffers(1, &vbo_fbo_tex_vertices);
	//
	//// generate a framebuffer 
	//glGenFramebuffers(1, &m_fbo);
	//// bind it as the target for rendering commands
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//
	//int width = 1024, height = 768;
	//
	//glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	//glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
	//glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 4);
	//
	////GLuint m_fb;
	////glGenRenderbuffers(1, &m_fb);
	////glBindRenderbuffer(GL_RENDERBUFFER, m_fb);
	////glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 768);
	////glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fb);
	//
	////GLuint m_colorTex[2];
	//// create n RGBA color textures
	//glGenTextures(2, m_colorTex);
	//
	//for (int i = 0; i < 2; ++i) {
	//	glBindTexture(GL_TEXTURE_2D, m_colorTex[i]);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	//		width, height,
	//		0, GL_RGBA, GL_UNSIGNED_BYTE,
	//		NULL);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//}
	//
	//// create a depth texture
	//glGenTextures(1, &m_depthTex);
	//glBindTexture(GL_TEXTURE_2D, m_depthTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	//	width, height,
	//	0, GL_DEPTH_COMPONENT, GL_FLOAT,
	//	NULL);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//// create the framebuffer object
	//glGenFramebuffers(1, &m_fbo);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//
	//// attach colors
	//for (int i = 0; i < 2; ++i) {
	//	glFramebufferTexture(GL_DRAW_FRAMEBUFFER,
	//		GL_COLOR_ATTACHMENT0 + i, m_colorTex[i], 0);
	//}
	//// attach color
	//glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);
	//
	//GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	//if (e != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	printf("There is a problem with the m_fbo\n");
	//	char ch;
	//	std::cin >> ch;
	//	exit(-1);
	//}
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
void OpenGLCanvas::SaveCameraParams()
{
	Camera* camera = m_renderer->GetCamera();
	std::ofstream fout("src/Configs/CameraParams.txt");
	fout << camera->camera_position.x << ' ' << camera->camera_position.y << ' ' << camera->camera_position.z << '\n';
	fout << camera->camera_look_at.x << ' ' << camera->camera_look_at.y << ' ' << camera->camera_look_at.z << '\n';
	fout << camera->camera_up.x << ' ' << camera->camera_up.y << ' ' << camera->camera_up.z << '\n';
	fout << camera->camera_pitch << ' ' << camera->camera_heading << ' ' << camera->camera_scale << '\n';

	fout.close();
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::paintGL()
{
	Camera* camera = m_renderer->GetCamera();
	
	if(m_cameraMoveFlags & (1 << (int)FORWARD))
	{
		camera->Move(FORWARD);
	}
	if (m_cameraMoveFlags & (1 << (int)LEFT))
	{
		camera->Move(LEFT);
	}
	if (m_cameraMoveFlags & (1 << (int)BACK))
	{
		camera->Move(BACK);
	}
	if (m_cameraMoveFlags & (1 << (int)RIGHT))
	{
		camera->Move(RIGHT);
	}
	if (m_cameraMoveFlags & (1 << (int)DOWN))
	{
		camera->Move(DOWN);
	}
	if (m_cameraMoveFlags & (1 << (int)UP))
	{
		camera->Move(UP);
	}

	camera->Update();
	m_renderer->Update();
	//m_mvp = camera->projection * camera->view;
	//m_time += 0.08;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//// Vao
	//glBindVertexArray(m_vao);
	
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	//// define the index array for the outputs
	//GLuint attachments[2] = { GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT/*, GL_COLOR_ATTACHMENT2*/ };
	//glDrawBuffers(2, attachments);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	////DrawSkybox(m_mvp);
	////DrawAxis(m_mvp);
	////DrawWater(m_mvp);
	////DrawFlowWater(m_mvp);
	//
	////DrawLightCube(m_mvp);
	////DrawScenePortalRectangle();
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
	//glUseProgram(m_filterShader);
	//
	//DrawFinalPass();
	f->glBindVertexArray(m_vao);
	m_renderer->DrawAll(defaultFramebufferObject());
}
///////////////////////////////////////////////////////////////////////////////////////
//void OpenGLCanvas::DrawLightCube(glm::mat4x4 &mvp)
//{
//	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
//	glUseProgram(m_lightCubeShader);
//
//	glBindBuffer(GL_ARRAY_BUFFER, m_lightPosVbo);
//	glBufferData(GL_ARRAY_BUFFER, m_lightCubeVertices.size() * sizeof(GLfloat), m_lightCubeVertices.data(), GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//
//	//glBindBuffer(GL_ARRAY_BUFFER, m_lightTextureCoordsVbo);
//	//glBufferData(GL_ARRAY_BUFFER, m_lightTexCoord.size() * sizeof(GLfloat), m_lightTexCoord.data(), GL_STATIC_DRAW);
//	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
//	//glEnableVertexAttribArray(1);
//
//	GLuint pos0Location = glGetUniformLocation(m_lightCubeShader, "pos0");
//	glUniform3fv(pos0Location, 1, glm::value_ptr(m_lightPosition));
//
//	GLuint mvpLocation = glGetUniformLocation(m_lightCubeShader, "mvp");
//	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
//
//	//glEnable(GL_TEXTURE0);
//	//GLuint sceneTextureLocation = glGetUniformLocation(m_lightCubeShader, "texFramebuffer");
//	//glUniform1i(sceneTextureLocation, 0);
//	//
//	//glActiveTexture(GL_TEXTURE0);
//	//glBindTexture(GL_TEXTURE_2D, m_colorTex[0]);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	// draw
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//}
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
	Camera* camera = m_renderer->GetCamera();
	if (m_mousePressed)
	{
		camera->Move2D(event->pos().x(), event->pos().y());
	}
	else
	{
		camera->mouse_position = glm::vec3(event->pos().x(), event->pos().y(), 0);
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
	Camera* camera = m_renderer->GetCamera();
	switch (event->key()) {
	case Qt::Key_W:
		if (m_lightModeView)
		{
			m_lightPosition += glm::vec3(0.5, 0.0, 0.0);
		}
		else
		{
			//camera->Move(FORWARD);
			m_cameraMoveFlags |= (1 << (int)FORWARD);
		}
		break;
	case Qt::Key_A:
		if (m_lightModeView)
		{
			m_lightPosition += glm::vec3(0.0, 0.0, -0.5);
		}
		else
		{
			//camera->Move(LEFT);
			m_cameraMoveFlags |= (1 << (int)LEFT);
		}
		break;
	case Qt::Key_S:
		if (m_lightModeView)
		{
			m_lightPosition += glm::vec3(-0.5, 0.0, 0.0);
		}
		else
		{
			//camera->Move(BACK);
			m_cameraMoveFlags |= (1 << (int)BACK);
		}
		break;
	case Qt::Key_D:
		if (m_lightModeView)
		{
			m_lightPosition += glm::vec3(0.0, 0.0, 0.5);
		}
		else
		{
			//camera->Move(RIGHT);
			m_cameraMoveFlags |= (1 << (int)RIGHT);
		}
		break;
	case Qt::Key_Q:
		if (m_lightModeView)
		{
			m_lightPosition += glm::vec3(0.0, -0.5, 0.0);
		}
		else
		{
			//camera->Move(DOWN);
			m_cameraMoveFlags |= (1 << (int)DOWN);
		}
		break;
	case Qt::Key_E:
		if (m_lightModeView)
		{
			m_lightPosition += glm::vec3(0.0, 0.5, 0.0);
		}
		else
		{
			//camera->Move(UP);
			m_cameraMoveFlags |= (1 << (int)UP);
		}
		break;
	case Qt::Key_T:
		m_isWireframeMode ^= 1;
		break;
	case Qt::Key_1:
	{
		if (event->modifiers() == Qt::ControlModifier)
			m_waterParams.A += 0.1;
		else
			m_waterParams.A -= 0.1;
		break;
	}
	case Qt::Key_2:
	{
		if (event->modifiers() == Qt::ControlModifier)
			m_waterParams.S += 0.1;
		else
			m_waterParams.S -= 0.1;
		break;
	}
	case Qt::Key_3:
	{
		if (event->modifiers() == Qt::ControlModifier)
			m_waterParams.W += 0.1;
		else
			m_waterParams.W -= 0.1;
		break;
	}
	case Qt::Key_4:
	{
		if (event->modifiers() == Qt::ControlModifier)
			m_waterParams.L += 0.1;
		else
			m_waterParams.L -= 0.1;
		m_waterParams.A = 0.03f * m_waterParams.L;
		break;
	}
	case Qt::Key_5:
	{
		if (event->modifiers() == Qt::ControlModifier)
			m_waterParams.Q += 0.1;
		else
			m_waterParams.Q -= 0.1;
		break;
	}
	case Qt::Key_P:
	{
		m_debugModeView = !m_debugModeView;
	}
	case Qt::Key_L:
	{
		m_lightModeView = !m_lightModeView;
	}
	case Qt::Key_Z:
	{
		targetCnt++;
	}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::keyReleaseEvent(QKeyEvent *event)
{
	Camera* camera = m_renderer->GetCamera();
	switch (event->key()) {
	case Qt::Key_W:
		{
			m_cameraMoveFlags &= ~(1 << (int)FORWARD);
		}
		break;
	case Qt::Key_A:
		{
			m_cameraMoveFlags &= ~(1 << (int)LEFT);
		}
		break;
	case Qt::Key_S:
		{
			m_cameraMoveFlags &= ~(1 << (int)BACK);
		}
		break;
	case Qt::Key_D:
		{
			m_cameraMoveFlags &= ~(1 << (int)RIGHT);
		}
		break;
	case Qt::Key_Q:
		{
			m_cameraMoveFlags &= ~(1 << (int)DOWN);
		}
		break;
	case Qt::Key_E:
		{
			m_cameraMoveFlags &= ~(1 << (int)UP);
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitGeometry()
{
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
		m_lightCubeVertices[i * 3] = m_skyboxVertices[i * 3] * m_lightCubeSize;
		m_lightCubeVertices[i * 3 + 1] = m_skyboxVertices[i * 3 + 1] * m_lightCubeSize;
		m_lightCubeVertices[i * 3 + 2] = m_skyboxVertices[i * 3 + 2] * m_lightCubeSize;
	}

	m_lightTexCoord.resize(36 * 2);
	for (int i = 0; i < 6; i += 12)
	{
		m_lightTexCoord[i * 8] = 0.0f;
		m_lightTexCoord[i * 8 + 1] = 0.0f;

		m_lightTexCoord[i * 8 + 2] = 1.0f;
		m_lightTexCoord[i * 8 + 3] = 0.0f;

		m_lightTexCoord[i * 8 + 4] = 1.0f;
		m_lightTexCoord[i * 8 + 5] = 1.0f;

		m_lightTexCoord[i * 8 + 6] = 1.0f;
		m_lightTexCoord[i * 8 + 7] = 1.0f;

		m_lightTexCoord[i * 8 + 8] = 0.0f;
		m_lightTexCoord[i * 8 + 9] = 1.0f;

		m_lightTexCoord[i * 10] = 0.0f;
		m_lightTexCoord[i * 8 + 11] = 0.0f;
	}

	for (int i = 0; i < 36 * 3; ++i)
		m_skyboxVertices[i] = m_skyboxVertices[i] * m_cubeSize;
}
/////////////////////////////////////////////////////////////////////////////////////
void OpenGLCanvas::InitWaterParams()
{
	float Q[4] = { 0.1f, 0.0001f, 0.0001f, 0.0001f };
	float S[4] = { 2.15f, 0.8f, 0.4f, 0.3f };
	float L[4] = { 15.f, 8.13f, 5.42f, 3.96f };
	float Af[4] = { 0.23f, 0.213f, 0.125f, 0.076f };

	m_wavesGeometricData.resize(24);
	int NR_WAVES = 4;
	for (int i = 0; i < NR_WAVES; ++i)
	{
		int idx = 6*i;
		m_wavesGeometricData[idx] = L[i];

		glm::vec2 dir(GetNextRand(), GetNextRand());
		dir = glm::normalize(dir);

		m_wavesGeometricData[idx + 1] = dir.x;
		m_wavesGeometricData[idx + 2] = dir.y;
		m_wavesGeometricData[idx + 3] = Q[i];
		m_wavesGeometricData[idx + 4] = S[i];
		m_wavesGeometricData[idx + 5] = Af[i] * L[i];
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
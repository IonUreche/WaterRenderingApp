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
#include "DirectionVectorWidget.h"

#include "Camera.h"
#include "ShaderManager.h"

#include "WaterIFFT.h"


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
	m_wid = wid;

	QVBoxLayout *vlay = new QVBoxLayout();
	vlay->setContentsMargins(1, 1, 1, 1);
	wid->setLayout(vlay);

	{
		QWidget *wid2 = new QWidget(wid);
		QHBoxLayout *hlay = new QHBoxLayout();
		hlay->setContentsMargins(1, 1, 1, 1);
		wid2->setLayout(hlay);

		vlay->addWidget(wid2);

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
			s1->setDecimals(4);
			s1->setMinimum(-10000);
			s1->setMaximum(10000);
			s1->setValue(m_wavesGeometricData[6*i]);
			m_wavesGeometricDataWidgets.push_back(s1);
			h1->addWidget(s1);
			QObject::connect(s1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
			{
				m_wavesGeometricData[6 * i] = newVal;
			});
			vlay->addWidget(w1);

			// Q Widget
			QWidget *w4 = new QWidget();
			QHBoxLayout *h4 = new QHBoxLayout();
			h4->setContentsMargins(1, 1, 1, 1);
			w4->setLayout(h4);
			QLabel *l4 = new QLabel();
			l4->setText("Q: ");
			h4->addWidget(l4);
			QDoubleSpinBox *s4 = new QDoubleSpinBox();
			s4->setDecimals(4);
			s4->setMinimum(-10000);
			s4->setMaximum(10000);
			s4->setValue(m_wavesGeometricData[6 * i + 1]);
			m_wavesGeometricDataWidgets.push_back(s4);
			h4->addWidget(s4);
			QObject::connect(s4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
			{
				m_wavesGeometricData[6 * i + 1] = newVal;
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
			s5->setDecimals(4);
			s5->setMinimum(-10000);
			s5->setMaximum(10000);
			s5->setValue(m_wavesGeometricData[6 * i + 2]);
			m_wavesGeometricDataWidgets.push_back(s5);
			h5->addWidget(s5);
			QObject::connect(s5, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
			{
				m_wavesGeometricData[6 * i + 2] = newVal;
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
			s6->setDecimals(4);
			s6->setMinimum(-10000);
			s6->setMaximum(10000);
			s6->setValue(m_wavesGeometricData[6 * i + 3]);
			m_wavesGeometricDataWidgets.push_back(s6);
			h6->addWidget(s6);
			QObject::connect(s6, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
			{
				m_wavesGeometricData[6 * i + 3] = newVal;
			});
			vlay->addWidget(w6);

// Dx
QWidget *w7 = new QWidget();
QHBoxLayout *h7 = new QHBoxLayout();
h7->setContentsMargins(1, 1, 1, 1);
w7->setLayout(h7);
QLabel *ldx = new QLabel();
ldx->setText("Dx: ");
h7->addWidget(ldx);
QDoubleSpinBox *sdx = new QDoubleSpinBox();
sdx->setDecimals(4);
sdx->setMinimum(-10000);
sdx->setMaximum(10000);
sdx->setValue(m_wavesGeometricData[6 * i + 4]);
m_wavesGeometricDataWidgets.push_back(sdx);
h7->addWidget(sdx);
QObject::connect(sdx, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
{
	m_wavesGeometricData[6 * i + 4] = newVal;
});
vlay->addWidget(w7);

// Dy
QWidget *w8 = new QWidget();
QHBoxLayout *h8 = new QHBoxLayout();
h8->setContentsMargins(1, 1, 1, 1);
w8->setLayout(h8);
QLabel *ldy = new QLabel();
ldy->setText("Dy: ");
h8->addWidget(ldy);
QDoubleSpinBox *sdy = new QDoubleSpinBox();
sdy->setDecimals(4);
sdy->setMinimum(-10000);
sdy->setMaximum(10000);
sdy->setValue(m_wavesGeometricData[6 * i + 5]);
m_wavesGeometricDataWidgets.push_back(sdy);
h8->addWidget(sdy);
QObject::connect(sdy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, i](double newVal)
{
	m_wavesGeometricData[6 * i + 5] = newVal;
});
vlay->addWidget(w8);

if (i == 0)
{
	m_configNameLineEdit = new QLineEdit();
	vlay->addWidget(m_configNameLineEdit);
}
else if (i == 1)
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
			//if(i % 6 != 4 && i % 6 != 5)
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
	}

	{
		QWidget *wid2 = new QWidget(wid);
		QVBoxLayout *v2lay = new QVBoxLayout();
		v2lay->setContentsMargins(1, 1, 1, 1);
		wid2->setLayout(v2lay);

		{
			QWidget *L_wid = new QWidget(wid2);
			QHBoxLayout *L_lay = new QHBoxLayout();
			L_wid->setLayout(L_lay);

			QLabel *L_label = new QLabel(L_wid);
			L_label->setText("L: ");
			L_lay->addWidget(L_label);

			QSlider *L_slider = new QSlider(Qt::Orientation::Horizontal, L_wid);
			L_slider->setMinimum(1);
			L_slider->setMaximum(20000);
			L_lay->addWidget(L_slider);

			QSpinBox *L_spin = new QSpinBox();
			L_spin->setMinimum(1);
			L_spin->setMaximum(20000);

			L_lay->addWidget(L_spin);
			QObject::connect(L_spin, QOverload<int>::of(&QSpinBox::valueChanged), [this, L_slider](int newVal)
			{
				m_renderer->GetWaterIFFT()->SetL(newVal);
				L_slider->setValue(newVal);
			});

			QObject::connect(L_slider, &QSlider::valueChanged, [this, L_spin](int newVal)
			{
				m_renderer->GetWaterIFFT()->SetL(newVal);
				L_spin->setValue(newVal);
			});

			v2lay->addWidget(L_wid);
		}

		{
			QWidget *A_wid = new QWidget(wid2);
			QHBoxLayout *A_lay = new QHBoxLayout();
			A_wid->setLayout(A_lay);

			QLabel *A_label = new QLabel(A_wid);
			A_label->setText("A: ");
			A_lay->addWidget(A_label);

			QSlider *A_slider = new QSlider(Qt::Orientation::Horizontal, A_wid);
			A_slider->setMinimum(0.01);
			A_slider->setMaximum(20000);
			A_lay->addWidget(A_slider);

			QDoubleSpinBox *A_spin = new QDoubleSpinBox();
			A_spin->setDecimals(4);
			A_spin->setMinimum(0.01);
			A_spin->setMaximum(20000);

			A_lay->addWidget(A_spin);
			QObject::connect(A_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, A_slider](double newVal)
			{
				m_renderer->GetWaterIFFT()->SetAmplitude(newVal);
				A_slider->setValue(newVal);
			});

			QObject::connect(A_slider, &QSlider::valueChanged, [this, A_spin](int newVal)
			{
				m_renderer->GetWaterIFFT()->SetAmplitude(newVal);
				A_spin->setValue(newVal);
			});

			v2lay->addWidget(A_wid);
		}

		{
			QWidget *I_wid = new QWidget(wid2);
			QHBoxLayout *I_lay = new QHBoxLayout();
			I_wid->setLayout(I_lay);

			QLabel *I_label = new QLabel(I_wid);
			I_label->setText("I: ");
			I_lay->addWidget(I_label);

			QSlider *I_slider = new QSlider(Qt::Orientation::Horizontal, I_wid);
			I_slider->setMinimum(0.01);
			I_slider->setMaximum(20000);
			I_lay->addWidget(I_slider);

			QDoubleSpinBox *I_spin = new QDoubleSpinBox();
			I_spin->setDecimals(4);
			I_spin->setMinimum(0.01);
			I_spin->setMaximum(20000);

			I_lay->addWidget(I_spin);
			QObject::connect(I_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, I_slider](double newVal)
			{
				m_renderer->GetWaterIFFT()->SetIntensity(newVal);
				I_slider->setValue(newVal);
			});

			QObject::connect(I_slider, &QSlider::valueChanged, [this, I_spin](int newVal)
			{
				m_renderer->GetWaterIFFT()->SetIntensity(newVal);
				I_spin->setValue(newVal);
			});

			v2lay->addWidget(I_wid);
		}

		{
			QWidget *C_wid = new QWidget(wid2);
			QHBoxLayout *C_lay = new QHBoxLayout();
			C_wid->setLayout(C_lay);

			QLabel *C_label = new QLabel(C_wid);
			C_label->setText("C: ");
			C_lay->addWidget(C_label);

			QSlider *C_slider = new QSlider(Qt::Orientation::Horizontal, C_wid);
			C_slider->setMinimum(0.01);
			C_slider->setMaximum(1000);
			C_lay->addWidget(C_slider);

			QDoubleSpinBox *C_spin = new QDoubleSpinBox();
			C_spin->setDecimals(4);
			C_spin->setSingleStep(0.1);
			C_spin->setMinimum(0.00);
			C_spin->setMaximum(200);

			C_lay->addWidget(C_spin);
			QObject::connect(C_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, C_slider](double newVal)
			{
				m_renderer->GetWaterIFFT()->SetCapillarSupressFactor(newVal);
				C_slider->setValue(newVal);
			});

			QObject::connect(C_slider, &QSlider::valueChanged, [this, C_spin](int newVal)
			{
				m_renderer->GetWaterIFFT()->SetCapillarSupressFactor(newVal);
				C_spin->setValue(newVal);
			});

			v2lay->addWidget(C_wid);
		}

		{
			DirectionVectorWidget *D_wid = new DirectionVectorWidget(wid2);
			
			QObject::connect(D_wid, &DirectionVectorWidget::onMousePressed2, [this](float x, float y)
			{
				m_renderer->GetWaterIFFT()->SetDirection(x, y);
			});

			v2lay->addWidget(D_wid);
		}
		vlay->addWidget(wid2);
	}
	//wid->hide();
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
void OpenGLCanvas::InitBuffers()
{
	f->glEnable(GL_DEPTH_TEST);
	f->glGenVertexArrays(1, &m_vao);
	f->glBindVertexArray(m_vao);
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

	f->glBindVertexArray(m_vao);
	m_renderer->SetGerstnerParams(m_wavesGeometricData);
	m_renderer->SetGerstnerNormalsParams(m_wavesGeometricData);
	m_renderer->SetDebugModeStatus(m_debugModeView);
	m_renderer->DrawAll(defaultFramebufferObject());
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
		m_renderer->SetRenderType(0);
		break;
	}
	case Qt::Key_2:
	{
		m_renderer->SetRenderType(1);
		break;
	}
	case Qt::Key_3:
	{
		m_renderer->SetRenderType(2);
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
		break;
	}
	case Qt::Key_L:
	{
		m_lightModeView = !m_lightModeView;
		break;
	}
	case Qt::Key_C:
	{
		m_renderer->ToggleChoppy();
		break;
	}
	case Qt::Key_Z:
	{
		targetCnt++;
		break;
	}
	case Qt::Key_G:
	{
		if (m_wid)
		{
			if (m_wid->isVisible())
			{
				m_wid->hide();
			}
			else
			{
				m_wid->show();
			}
		}
		break;
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

	int NR_WAVES = 4;
	m_wavesGeometricData.resize(NR_WAVES * 6);
	for (int i = 0; i < NR_WAVES; ++i)
	{
		int idx = 6*i;
		m_wavesGeometricData[idx] = L[i];

		glm::vec2 dir(GetNextRand(), GetNextRand() * 2 - 1);
		dir = glm::normalize(dir);

		//m_wavesGeometricData[idx + 1] = dir.x;
		//m_wavesGeometricData[idx + 2] = dir.y;
		//m_wavesGeometricData[idx + 3] = Q[i];
		//m_wavesGeometricData[idx + 4] = S[i];
		//m_wavesGeometricData[idx + 5] = Af[i] / L[i];

		m_wavesGeometricData[idx + 1] = Q[i];
		m_wavesGeometricData[idx + 2] = S[i];
		m_wavesGeometricData[idx + 3] = Af[i] / L[i];
		m_wavesGeometricData[idx + 4] = dir.x;
		m_wavesGeometricData[idx + 5] = dir.y;

		// 1 -> 4
		// 2 -> 5
		// 3 -> 1
		// 4 -> 2
		// 5 -> 3
	}

	int NR_NM_WAVES = 50;
	m_wavesNormalMapData.resize(NR_NM_WAVES * 6);
	for (int i = 0; i < NR_NM_WAVES; ++i)
	{
	/*
		float wl = m_nm_waves[i].params.wavelength = (frandf() * 0.5f + 0.3f);
		//#define SHITTY_TILE
#ifdef SHITTY_TILE
		// tile but shitty
		m_nm_waves[i].params.wave_dir = (Vector2::randomDirection()*6.f).floor() / 2.f * wl;
#else   
		// not shitty but not tiled
		m_nm_waves[i].params.wave_dir = (Vector2::randomDirection());
#endif
		m_nm_waves[i].params.steepness = 5.f*(frandf() * 2.f + 1.f);
		m_nm_waves[i].params.speed = 0.05f * sqrt(M_PI / wl);
		m_nm_waves[i].params.kAmpOverLen = 0.03f;
		*/

		int idx = 6 * i;
		float wl = (GetNextRand() * 0.5f + 0.3f);
		m_wavesNormalMapData[idx] = wl;
		
		m_wavesNormalMapData[idx + 1] = 5.f * (GetNextRand() * 2.f + 1.f);
		m_wavesNormalMapData[idx + 2] = 0.05f * sqrt(M_PI / wl);
		m_wavesNormalMapData[idx + 3] = 0.03f;

		glm::vec2 dir(GetNextRand(), GetNextRand());
		dir = glm::normalize(dir);
		m_wavesNormalMapData[idx + 4] = dir.x;
		m_wavesNormalMapData[idx + 5] = dir.y;
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
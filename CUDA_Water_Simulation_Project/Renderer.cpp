#include "Renderer.h"
#include "ShaderManager.h"

#include <fstream>

Renderer::Renderer()
{
	initializeOpenGLFunctions();
	ShaderManager* inst = ShaderManager::GetInstance();
}
//////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitShaders()
{
	ShaderManager* inst = ShaderManager::GetInstance();
	inst->InitShaders();
}
//////////////////////////////////////////////////////////////////////////
void Renderer::InitCamera(int canvasWidth, int canvasHeight)
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
	camera.camera_pitch = x;
	camera.camera_heading = y;
	camera.camera_scale = z;
	fin.close();

	int canvasWidth = canvasWidth;
	int canvasHeight = canvasHeight;
	float width = canvasWidth, height = canvasHeight, znear = 0.1f, zfar = 20000.0f, fov = 45;

	//camera.SetProgramId(m_axisShader);
	camera.SetPosition(Obs);
	camera.SetLookAt(RefPct);
	camera.camera_up = cameraUp;

	camera.SetFOV(fov);
	camera.aspect = GLfloat(width) / GLfloat(height);
	camera.near_clip = znear;
	camera.far_clip = zfar;
	camera.move_camera = true;
}
//////////////////////////////////////////////////////////////////////////
void Renderer::DrawAll()
{
	for (auto obj : m_objects)
	{
		obj->Draw();
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
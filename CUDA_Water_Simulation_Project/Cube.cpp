#include "Cube.h"



Cube::Cube(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
///////////////////////////////////////////////////////////////
Cube::~Cube()
{
}
///////////////////////////////////////////////////////////////
void Cube::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
	f->glGenBuffers(1, &m_colorsBufferId);
}
///////////////////////////////////////////////////////////////
void Cube::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode)
{
	GLuint m_cubeShader = ShaderManager::GetInstance()->GetShaderProgram("basic3D");

	f->glUseProgram(m_cubeShader);

	f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	f->glBufferData(GL_ARRAY_BUFFER, m_lightCubeVertices.size() * sizeof(GLfloat), m_lightCubeVertices.data(), GL_STATIC_DRAW);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(0);

	f->glBindBuffer(GL_ARRAY_BUFFER, m_colorsBufferId);
	f->glBufferData(GL_ARRAY_BUFFER, m_colorVertices.size() * sizeof(GLfloat), m_colorVertices.data(), GL_STATIC_DRAW);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(1);

	GLint pos0Location = f->glGetUniformLocation(m_cubeShader, "pos0");
	f->glUniform3fv(pos0Location, 1, glm::value_ptr(m_position));

	GLint mvpLocation = f->glGetUniformLocation(m_cubeShader, "mvp");
	f->glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

	f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// draw
	f->glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Cube::InitGeometry()
{
	m_cubeVertices = {
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

	m_colorVertices = {
		// Positions          
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};

	m_lightCubeVertices.resize(36 * 3);
	//m_colorVertices.resize(36 * 3);
	for (int i = 0; i < 36; ++i)
	{
		m_lightCubeVertices[i * 3] = m_cubeVertices[i * 3] * m_CubeSize;
		m_lightCubeVertices[i * 3 + 1] = m_cubeVertices[i * 3 + 1] * m_CubeSize;
		m_lightCubeVertices[i * 3 + 2] = m_cubeVertices[i * 3 + 2] * m_CubeSize;

		//m_colorVertices[i * 3] = 1.0f;
		//m_colorVertices[i * 3 + 1] = 1.0f;
		//m_colorVertices[i * 3 + 2] = 1.0f;
	}
}
///////////////////////////////////////////////////////////////
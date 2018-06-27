#include "WorldGizmo.h"



WorldGizmo::WorldGizmo(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
///////////////////////////////////////////////////////////////
WorldGizmo::~WorldGizmo()
{
}
///////////////////////////////////////////////////////////////
void WorldGizmo::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
	f->glGenBuffers(1, &m_colorsBufferId);
}
///////////////////////////////////////////////////////////////
void WorldGizmo::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode)
{
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//	// Shader
	GLuint axisShader = ShaderManager::GetInstance()->GetShaderProgram("basic3D");

	f->glUseProgram(axisShader);
	
	// Uniforms
	GLint uniMvp = f->glGetUniformLocation(axisShader, "mvp");
	f->glUniformMatrix4fv(uniMvp, 1, GL_FALSE, glm::value_ptr(mvp));
	
	GLuint pos0Location = f->glGetUniformLocation(axisShader, "pos0");
	f->glUniform3fv(pos0Location, 1, glm::value_ptr(glm::vec3(0.0)));
	
	// position
	f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	f->glBufferData(GL_ARRAY_BUFFER, m_axisCoords.size() * sizeof(GLfloat), m_axisCoords.data(), GL_STATIC_DRAW);
	f->glEnableVertexAttribArray(0);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	// color
	f->glBindBuffer(GL_ARRAY_BUFFER, m_colorsBufferId);
	f->glBufferData(GL_ARRAY_BUFFER, m_coordinateAxisColors.size() * sizeof(GLfloat), m_coordinateAxisColors.data(), GL_STATIC_DRAW);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	// draw 
	f->glDrawArrays(GL_LINES, 0, m_axisCoords.size());
}

void WorldGizmo::InitGeometry()
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
}
///////////////////////////////////////////////////////////////
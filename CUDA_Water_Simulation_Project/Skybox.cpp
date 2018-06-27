#include "Skybox.h"

#include "TextureManager.h"

Skybox::Skybox(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
//////////////////////////////////////////////////////////////////////////
Skybox::~Skybox()
{
}
//////////////////////////////////////////////////////////////////////////
void Skybox::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
}
//////////////////////////////////////////////////////////////////////////
void Skybox::InitGeometry()
{
	GLfloat skyboxVertices[] = {
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
	m_vertices.resize(36 * 3);
	for (int i = 0; i < 36 * 3; ++i)
		m_vertices[i] = skyboxVertices[i] * m_size;
}
//////////////////////////////////////////////////////////////////////////
void Skybox::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode)
{
	f->glDepthMask(GL_FALSE);
	auto skyboxShader = ShaderManager::GetInstance()->GetShaderProgram("skybox");
	f->glUseProgram(skyboxShader);

	f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	f->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(0);

	GLint mvpLocation = f->glGetUniformLocation(skyboxShader, "mvp");
	f->glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint OldCullFaceMode;
	f->glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	f->glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	f->glCullFace(GL_FRONT);
	f->glDepthFunc(GL_LEQUAL);

	auto skyboxTexture = TextureManager::GetInstance()->GetTexture("cubemap");

	f->glActiveTexture(GL_TEXTURE0);
	f->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	f->glUniform1i(f->glGetUniformLocation(skyboxShader, "skybox"), 0);

	f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	f->glDrawArrays(GL_TRIANGLES, 0, 36);

	f->glDepthMask(GL_TRUE);
	f->glCullFace(OldCullFaceMode);
	f->glDepthFunc(OldDepthFuncMode);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "ScreenQuad.h"

#include "TextureManager.h"


ScreenQuad::ScreenQuad(QOpenGLContext *glContext)
{
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
///////////////////////////////////////////////////////////////
ScreenQuad::~ScreenQuad()
{
}
///////////////////////////////////////////////////////////////
void ScreenQuad::InitBuffers()
{
	f->glGenBuffers(1, &vbo_fbo_vertices);
	f->glGenBuffers(1, &vbo_fbo_tex_vertices);
}
///////////////////////////////////////////////////////////////
void ScreenQuad::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos)
{
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_renderTarget);
	std::string shaderName = "customEffect";
	if (m_type == 1)
	{
		shaderName = "textureDisplay1D";
	}
	if (m_type == 2)
	{
		shaderName = "textureDisplay2D";
	}

	GLint basicShader = ShaderManager::GetInstance()->GetShaderProgram(shaderName);
	f->glUseProgram(basicShader);

	GLint timeLocation = f->glGetUniformLocation(basicShader, "time");
	if (timeLocation != -1)
	{
		f->glUniform1f(timeLocation, m_time);
	}

	GLint flipVLocation = f->glGetUniformLocation(basicShader, "flipV");
	if (flipVLocation != -1)
	{
		f->glUniform1i(flipVLocation, m_flipV);
	}

	GLenum glGetError1;
	glGetError1 = f->glGetError();

	m_time += 0.02f;
	
	//GLfloat fbo_vertices[] = {
	//	-1.0f, -1.0f,
	//	1.0f, -1.0f,
	//	-1.0f, 1.0f,
	//	1.0f, 1.0f,
	//};
	
	GLfloat fbo_tex_vertices[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		};
	
	f->glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	f->glBufferData(GL_ARRAY_BUFFER, m_fbo_vertices.size() * sizeof(GLfloat), m_fbo_vertices.data(), GL_STATIC_DRAW);
	f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(0);
	
	f->glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_tex_vertices);
	f->glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_tex_vertices), fbo_tex_vertices, GL_STATIC_DRAW);
	f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(1);

	glGetError1 = f->glGetError();
	
	glEnable(GL_TEXTURE0);
	GLint sceneTextureLocation = f->glGetUniformLocation(basicShader, "renderedTexture");
	if (sceneTextureLocation != -1)
	{
		GLuint grassTexture = TextureManager::GetInstance()->GetTexture("grass");
		f->glActiveTexture(GL_TEXTURE0);
		f->glBindTexture(GL_TEXTURE_2D, /*grassTexture*/m_colorAttachment0);
		f->glUniform1i(sceneTextureLocation, 0);
	}

	glGetError1 = f->glGetError();

	//if (m_isWireframeMode)
	//{
		//f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//}
	//else
	//{
		f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//}
	f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScreenQuad::InitGeometry()
{
}
///////////////////////////////////////////////////////////////
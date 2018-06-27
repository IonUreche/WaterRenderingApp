#pragma once
#include "IDrawable.h"
#include <vector>

class ScreenQuad : public IDrawable
{
public:
	ScreenQuad(QOpenGLContext *glContext);
	~ScreenQuad();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode = false) override;

	void SetRenderTarget(GLuint renderTarget){ m_renderTarget = renderTarget; }
	void SetColorAttachment(GLuint colorAttachment) { m_colorAttachment0 = colorAttachment; }

	void SetFBOVertices(std::vector<GLfloat> vert) { m_fbo_vertices = vert; }

	void SetType(int type){ m_type = type; }
	void SetFlipV(int flipV) { m_flipV = flipV; }

private:

	int m_type = 0;

	std::vector<GLfloat> m_fbo_vertices;

	GLuint vbo_fbo_vertices;
	GLuint vbo_fbo_tex_vertices;

	GLuint m_renderTarget;
	GLuint m_colorAttachment0;
	float m_time = 0;
	int m_flipV = 0;
};


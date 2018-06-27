#pragma once
#include "IDrawable.h"

class Skybox : public IDrawable
{
public:
	Skybox(QOpenGLContext *glContext, GLuint fbo);
	~Skybox();

	void InitBuffers() override;
	void InitGeometry() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode = false) override;
	void SetSize(float size){ m_size = size; }

protected:
	GLuint m_vbo;
	std::vector<GLfloat> m_vertices;
	float m_size;
};


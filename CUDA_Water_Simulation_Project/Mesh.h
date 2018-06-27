#pragma once
#include "IDrawable.h"

class Mesh : public IDrawable
{
public:
	Mesh(QOpenGLContext *glContext);
	~Mesh();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode = false) override;

protected:
	GLuint m_vbo;
	std::vector<GLfloat> m_patch_vert;
};


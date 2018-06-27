#pragma once
#include "IDrawable.h"
#include <vector>
#include "glm/vec4.hpp"

class Terrain : public IDrawable 
{
public:
	Terrain(QOpenGLContext *glContext, GLuint fbo);
	~Terrain();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode = false) override;
	void SetPlane(glm::vec4 plane) { m_plane = plane; }

private:
	GLuint m_vbo;
	std::vector<GLfloat> m_patch_vert;
	glm::vec4 m_plane;
};


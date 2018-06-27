#pragma once
#include "IDrawable.h"
#include <vector>

class WorldGizmo : public IDrawable
{
public:
	WorldGizmo(QOpenGLContext *glContext, GLuint fbo);
	~WorldGizmo();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode = false) override;

private:
	std::vector<GLfloat> m_coordinateAxisColors;
	std::vector<GLfloat> m_axisCoords;

	GLuint m_vbo;
	GLuint m_colorsBufferId;
};


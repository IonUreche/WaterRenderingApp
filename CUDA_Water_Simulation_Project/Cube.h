#pragma once
#include "IDrawable.h"
#include <vector>
#include "glm/vec3.hpp"

class Cube : public IDrawable
{
public:
	Cube(QOpenGLContext *glContext, GLuint fbo);
	~Cube();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos) override;
	void SetCubeSize(float cubeSize){ m_CubeSize = cubeSize; }
	void SetPosition(glm::vec3 position) { m_position = position; }
	glm::vec3 GetPosition() const { return m_position; }

private:
	std::vector<GLfloat> m_lightCubeVertices;

	GLuint m_vbo;
	GLuint m_colorsBufferId;

	std::vector<GLfloat> m_cubeVertices;
	std::vector<GLfloat> m_colorVertices;

	glm::vec3 m_position;

	float m_CubeSize = 4;
};


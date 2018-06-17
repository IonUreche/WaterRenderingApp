#pragma once
#include "IDrawable.h"
#include "glm/vec4.hpp"

class Water : public IDrawable
{
public:
	Water(QOpenGLContext *glContext, GLuint fbo);
	~Water();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos) override;
	void SetPlane(glm::vec4 plane) { m_plane = plane; }
	void SetWaterTextures(GLuint tex1, GLuint tex2){ m_colorTex0 = tex1; m_colorTex1 = tex2; }
	void SetDepthTexture(GLuint depthTex) { m_depthTex = depthTex; }
	glm::mat4 m_view;

private:
	GLuint m_vbo;
	std::vector<GLfloat> m_patch_vert;
	glm::vec4 m_plane;
	glm::vec3 m_lightPosition;
	glm::vec2 m_screenResolution;

	float m_time = 0;

	GLuint m_colorTex0;
	GLuint m_colorTex1;
	GLuint m_depthTex;
};


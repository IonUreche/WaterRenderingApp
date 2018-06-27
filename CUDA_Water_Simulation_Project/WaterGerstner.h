#pragma once
#include "IDrawable.h"
#include "glm/vec4.hpp"

class WaterGerstner : public IDrawable
{
public:
	WaterGerstner(QOpenGLContext *glContext, GLuint fbo);
	~WaterGerstner();

	void InitGeometry() override;
	void InitBuffers() override;
	void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode = false) override;
	void SetPlane(glm::vec4 plane) { m_plane = plane; }
	void SetWaterTextures(GLuint tex1, GLuint tex2){ m_colorTex0 = tex1; m_colorTex1 = tex2; }
	void SetDepthTexture(GLuint depthTex) { m_depthTex = depthTex; }
	void SetGerstnerParams(std::vector<float> waterParams) { m_waterParams = waterParams; }
	void SetGerstnernormalsParams(std::vector<float> waterNormalsParams) { m_waterNormalsParams = waterNormalsParams; }
	void SetProjectionMatrix(glm::mat4 cameraMatProjection) { m_cameraProjection = cameraMatProjection; }
	void SetSkyboxCubeMap(GLuint skyboxTexture){ m_cubeMap = skyboxTexture; }


	glm::mat4 m_view;

private:
	GLuint m_vbo;
	std::vector<GLfloat> m_patch_vert;
	glm::vec4 m_plane;
	glm::vec3 m_lightPosition;
	glm::vec2 m_screenResolution;

	glm::mat4 m_cameraProjection;

	std::vector<float> m_waterParams;
	std::vector<float> m_waterNormalsParams;

	float m_time = 0;

	GLuint m_colorTex0;
	GLuint m_colorTex1;
	GLuint m_depthTex;
	GLuint m_cubeMap;
};


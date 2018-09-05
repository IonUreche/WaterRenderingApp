#pragma once
#include "IDrawable.h"
#include "glm/vec4.hpp"

class WaterIFFT : public IDrawable
{
public:
	WaterIFFT(QOpenGLContext *glContext, GLuint fbo);
	~WaterIFFT();

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

	//int m_N = 512;
	//int m_log_2_N = 9;
	//int m_L = 1000;
	void SetL(int L){ m_L = L; }
	void SetAmplitude(float amplitude) { m_amplitude = amplitude; }
	void SetIntensity(float intensity) { m_intensity = intensity; }
	void SetCapillarSupressFactor(float factor) { m_capillarSupressFactor = factor; }
	void SetDirection(float dx, float dy){ m_dir[0] = dx; m_dir[1] = dy; }
	void SetTiles(float tiles){ m_tiles = tiles; }

	GLuint GetTildeh0k() const { return m_tilde_h0k; }
	GLuint GetTildeh0minusk() const { return m_tilde_h0minusk; }
	GLuint GetTildehktX() const { return m_tilde_hkt_dx; }
	GLuint GetTildehktY() const { return m_tilde_hkt_dy; }
	GLuint GetTildehktZ() const { return m_tilde_hkt_dz; }
	GLuint GetTwiddleIndices() const { return m_twiddleIndices; }
	GLuint GetDyTexture() const { return m_DyTexture; }
	GLuint GetDxTexture() const { return m_DxTexture; }
	GLuint GetDzTexture() const { return m_DzTexture; }
	GLuint GetPingPongTexture(int i)
	{
		if (i == 1)
		{
			return m_pingpong0;
		}

		//return m_pingpong0;
		return m_tilde_hkt_dy;
	}

	void ComputeButterflyInversion(GLuint tildehtkTexture, GLuint DisplTexture);

	void SetDt(float dt){ m_time += dt; }
	void SetChoppy(bool val){ m_choppy = val; }
	void ToogleChoppy(){ m_choppy = !m_choppy; }

	std::vector<int> InitBitReversedIndices();
	unsigned int reverseBits(unsigned int n);

	glm::mat4 m_view;
protected:

	GLuint CreateTexture(int Width, int Height);

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

	GLuint m_ssbo;

	GLuint m_colorTex0;
	GLuint m_colorTex1;
	GLuint m_depthTex;
	GLuint m_cubeMap;

	GLuint m_tilde_h0k;
	GLuint m_tilde_h0minusk;

	GLuint m_tilde_hkt_dx;
	GLuint m_tilde_hkt_dy;
	GLuint m_tilde_hkt_dz;

	GLuint m_twiddleIndices;
	GLuint m_pingpong0;
	GLuint m_DyTexture;
	GLuint m_DxTexture;
	GLuint m_DzTexture;

	bool m_choppy = true;

	std::vector<int> m_bitReversedIndices;

	int pingpong;


	int m_N = 512;
	int m_log_2_N = 9;
	int m_L = 139;//1000;
	float m_amplitude = 4267;//= 20.0f;
	float m_intensity = 22;//5.0f;
	float m_dir[2] = { 1.0f, 0.0f };
	float m_capillarSupressFactor = 0.5f;
	float m_tiles = 1;
};
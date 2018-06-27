#include "WaterIFFT.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"

#include "TextureManager.h"

WaterIFFT::WaterIFFT(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
////////////////////////////////////////////////////////////////////////
WaterIFFT::~WaterIFFT()
{
	m_N = 512;
	m_log_2_N = (int)(log(m_N) / log(2));
	m_L = 1000;
	m_amplitude = 20.0f;
	m_intensity = 5.0f;
	m_dir[0] = 1.0f;
	m_dir[1] = 0.0f;
	m_capillarSupressFactor = 0.5f;
}
////////////////////////////////////////////////////////////////////////
void WaterIFFT::InitGeometry()
{
	// init patch vertices
	m_patch_vert =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	m_lightPosition = glm::vec3(200, 70, 200);
	m_screenResolution = glm::vec2(1024, 768);

	int tex_w = 512, tex_h = 512;

	m_tilde_h0k = CreateTexture(tex_w, tex_h);
	m_tilde_h0minusk = CreateTexture(tex_w, tex_h);

	m_tilde_hkt_dx = CreateTexture(tex_w, tex_h);
	m_tilde_hkt_dy = CreateTexture(tex_w, tex_h);
	m_tilde_hkt_dz = CreateTexture(tex_w, tex_h);

	m_twiddleIndices = CreateTexture(9, tex_h);
	m_pingpong0 = CreateTexture(tex_w, tex_h);

	m_DyTexture = CreateTexture(tex_w, tex_h);

	m_bitReversedIndices = InitBitReversedIndices();

	f->glGenBuffers(1, &m_ssbo);
	f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	f->glBufferData(GL_SHADER_STORAGE_BUFFER, m_bitReversedIndices.size() * sizeof(int), m_bitReversedIndices.data(), GL_DYNAMIC_COPY);
	f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

GLuint WaterIFFT::CreateTexture(int Width, int Height)
{
	// dimensions of the image
	
	GLuint tex_output;
	f->glGenTextures(1, &tex_output);
	f->glActiveTexture(GL_TEXTURE0);
	f->glBindTexture(GL_TEXTURE_2D, tex_output);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Width, Height, 0, GL_RGBA, GL_FLOAT,
		NULL);
	//f->glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	return tex_output;
}
////////////////////////////////////////////////////////////////////////
void WaterIFFT::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
}
////////////////////////////////////////////////////////////////////////
void WaterIFFT::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode)
{
	//m_time += 0.1f;
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	//m_lightPosition.x = std::sin(m_time * 20) * 100;
	//m_lightPosition.z = std::cos(m_time * 20) * 100;
	//m_lightPosition.y = 50 + std::cos(m_time * 20) * 50;
	
	GLuint h0k_shader = ShaderManager::GetInstance()->GetShaderProgram("h0k");
	GLuint noise0Texture = TextureManager::GetInstance()->GetTexture("Noise512_0");
	GLuint noise1Texture = TextureManager::GetInstance()->GetTexture("Noise512_1");
	GLuint noise2Texture = TextureManager::GetInstance()->GetTexture("Noise512_2");
	GLuint noise3Texture = TextureManager::GetInstance()->GetTexture("Noise512_3");
	
	GLenum glGetError1 = f->glGetError();

	// DRAW TERRAIN PATCHES
	f->glUseProgram(h0k_shader);
	glGetError1 = f->glGetError();
	//
	//GLint reflLocation = f->glGetUniformLocation(waterflowShader, "reflectionTexture");
	//glGetError1 = f->glGetError();
	//GLint refrLocation = f->glGetUniformLocation(waterflowShader, "refractionTexture");
	//glGetError1 = f->glGetError();

	// Texture unit 0 is for base images.
	//glUniform1i(grassLocation, 2); // Texture unit 2 is for normal maps.
	//glUniform1i(rockLocation, 3); // Texture unit 4 is for shadow maps.
	//glEnable(GL_TEXTURE4);
	//glEnable(GL_TEXTURE1);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	//glBindSampler(0, linearFiltering);

	//GLint heightMapLocation = f->glGetUniformLocation(terrainShader, "hhh");
	GLint th0kLocation = f->glGetUniformLocation(h0k_shader, "tilde_h0k");
	GLint th0kminusLocation = f->glGetUniformLocation(h0k_shader, "tilde_h0minusk");

	GLint noise1Location = f->glGetUniformLocation(h0k_shader, "noise_r0");
	GLint noise1iLocation = f->glGetUniformLocation(h0k_shader, "noise_i0");
	GLint noise2Location = f->glGetUniformLocation(h0k_shader, "noise_r1");
	GLint noise2iLocation = f->glGetUniformLocation(h0k_shader, "noise_i1");

	GLint NLocation = f->glGetUniformLocation(h0k_shader, "N");
	GLint LLocation = f->glGetUniformLocation(h0k_shader, "L");
	GLint amplitudeLocation = f->glGetUniformLocation(h0k_shader, "amplitude");
	GLint intensityLocation = f->glGetUniformLocation(h0k_shader, "intensity");
	GLint directionLocation = f->glGetUniformLocation(h0k_shader, "direction");
	GLint capillarSupressFactorLocation = f->glGetUniformLocation(h0k_shader, "capillarSupressFactor");

	//variables
	if (NLocation != -1)
	{
		f->glUniform1i(NLocation, m_N);
	}

	if (LLocation != -1)
	{
		f->glUniform1i(LLocation, m_N);
	}

	if (amplitudeLocation != -1)
	{
		f->glUniform1f(amplitudeLocation, m_amplitude);
	}

	if (intensityLocation != -1)
	{
		f->glUniform1f(intensityLocation, m_intensity);
	}
	
	if (directionLocation != -1)
	{
		f->glUniform2fv(directionLocation, 1, m_dir);
	}

	if (capillarSupressFactorLocation != -1)
	{
		f->glUniform1f(capillarSupressFactorLocation, m_capillarSupressFactor);
	}

	// textures
	if (th0kLocation != -1)
	{
		//f->glUniform1i(th0kLocation, 0);
		f->glBindImageTexture(0, m_tilde_h0k, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}
	
	if (th0kminusLocation != -1)
	{
		//f->glUniform1i(th0kLocation, 1);
		f->glBindImageTexture(1, m_tilde_h0minusk, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	if (noise1Location != -1)
	{
		f->glEnable(GL_TEXTURE0 + 2);
		f->glActiveTexture(GL_TEXTURE0 + 2);
		f->glBindTexture(GL_TEXTURE_2D, noise0Texture);
		f->glUniform1i(noise1Location, 2);
	}
	if (noise1iLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 3);
		f->glActiveTexture(GL_TEXTURE0 + 3);
		f->glBindTexture(GL_TEXTURE_2D, noise1Texture);
		f->glUniform1i(noise1iLocation, 3);
	}
	if (noise2Location != -1)
	{
		f->glEnable(GL_TEXTURE0 + 4);
		f->glActiveTexture(GL_TEXTURE0 + 4);
		f->glBindTexture(GL_TEXTURE_2D, noise2Texture);
		f->glUniform1i(noise2Location, 4);
	}
	if (noise2iLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 5);
		f->glActiveTexture(GL_TEXTURE0 + 5);
		f->glBindTexture(GL_TEXTURE_2D, noise3Texture);
		f->glUniform1i(noise2iLocation, 5);
	}

	//int tex_w = 512, tex_h = 512;
	f->glDispatchCompute((GLuint)m_N/16, (GLuint)m_N/16, 1);

	// make sure writing to image has finished before read
	f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	GLuint hkt_shader = ShaderManager::GetInstance()->GetShaderProgram("hkt");

	f->glUseProgram(hkt_shader);
	glGetError1 = f->glGetError();

	GLint th0kLocation2 = f->glGetUniformLocation(hkt_shader, "tilde_h0k");
	GLint th0kminusLocation2 = f->glGetUniformLocation(hkt_shader, "tilde_h0minusk");
	GLint hkt_dx_Location = f->glGetUniformLocation(hkt_shader, "tilde_hkt_dx");
	GLint hkt_dy_Location = f->glGetUniformLocation(hkt_shader, "tilde_hkt_dy");
	GLint hkt_dz_Location = f->glGetUniformLocation(hkt_shader, "tilde_hkt_dz");

	GLint N2Location = f->glGetUniformLocation(hkt_shader, "N");
	GLint L2Location = f->glGetUniformLocation(hkt_shader, "L");
	GLint tLocation = f->glGetUniformLocation(hkt_shader, "t");

	//variables
	if (N2Location != -1)
	{
		f->glUniform1i(N2Location, m_N);
	}

	if (L2Location != -1)
	{
		f->glUniform1i(L2Location, m_L);
	}

	if (tLocation != -1)
	{
		f->glUniform1f(tLocation, m_time);
	}

	// textures
	if (hkt_dy_Location != -1)
	{
		//f->glUniform1i(th0kLocation, 0);
		f->glBindImageTexture(0, m_tilde_hkt_dy, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	if (hkt_dx_Location != -1)
	{
		//f->glUniform1i(th0kLocation, 1);
		f->glBindImageTexture(1, m_tilde_hkt_dx, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	if (hkt_dz_Location != -1)
	{
		//f->glUniform1i(th0kLocation, 0);
		f->glBindImageTexture(2, m_tilde_hkt_dz, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	if (th0kLocation2 != -1)
	{
		//f->glUniform1i(th0kLocation, 1);
		f->glBindImageTexture(3, m_tilde_h0k, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	if (th0kminusLocation2 != -1)
	{
		//f->glUniform1i(th0kLocation, 0);
		f->glBindImageTexture(4, m_tilde_h0minusk, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	f->glDispatchCompute((GLuint)m_N / 16, (GLuint)m_N / 16, 1);

	// make sure writing to image has finished before read
	f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	// Twiddle
	GLuint twiddle_shader = ShaderManager::GetInstance()->GetShaderProgram("twiddleFactors");

	f->glUseProgram(twiddle_shader);
	glGetError1 = f->glGetError();

	GLint N3Location = f->glGetUniformLocation(twiddle_shader, "N");
	GLint twiddleIndicesLocation = f->glGetUniformLocation(twiddle_shader, "twiddleIndices");

	//variables
	if (N3Location != -1)
	{
		f->glUniform1i(N3Location, m_N);
	}

	// textures
	if (twiddleIndicesLocation != -1)
	{
		f->glBindImageTexture(0, m_twiddleIndices, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo);

	f->glDispatchCompute((GLuint)m_log_2_N, (GLuint)m_N / 16, 1);

	// make sure writing to image has finished before read
	f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// butterfly
	//GLuint butterfly_shader = ShaderManager::GetInstance()->GetShaderProgram("butterfly");
	//
	//f->glUseProgram(butterfly_shader);
	//glGetError1 = f->glGetError();
	//
	//GLint twiddlesIndices_Location = f->glGetUniformLocation(butterfly_shader, "twiddlesIndices");
	//GLint pingpong0_Location = f->glGetUniformLocation(butterfly_shader, "pingpong0");
	//GLint pingpong1_Location = f->glGetUniformLocation(butterfly_shader, "pingpong1");
	//
	//if (twiddlesIndices_Location != -1)
	//{
	//	f->glBindImageTexture(0, m_twiddleIndices, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//}
	//
	//if (pingpong0_Location != -1)
	//{
	//	f->glBindImageTexture(1, m_tilde_hkt_dy, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//}
	//
	//if (pingpong1_Location != -1)
	//{
	//	f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//}
	//
	//GLint stageLocation = f->glGetUniformLocation(butterfly_shader, "stage");
	//GLint pingpongLocation = f->glGetUniformLocation(butterfly_shader, "pingpong");
	//GLint directionLocation2 = f->glGetUniformLocation(butterfly_shader, "direction");
	//
	//// 1D FFT horizontal 
	//pingpong = 0;
	//for (int i = 0; i<m_log_2_N; i++)
	//{
	//	//variables
	//	if (stageLocation != -1)
	//	{
	//		f->glUniform1i(stageLocation, i);
	//	}
	//
	//	if (pingpongLocation != -1)
	//	{
	//		f->glUniform1i(pingpongLocation, pingpong);
	//	}
	//
	//	if (directionLocation2 != -1)
	//	{
	//		f->glUniform1i(directionLocation2, 0);
	//	}
	//	
	//	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
	//	f->glFinish();
	//	pingpong++;
	//	pingpong %= 2;
	//}
	//
	////1D FFT vertical 
	//for (int j = 0; j<m_log_2_N; j++)
	//{
	//	//variables
	//	if (stageLocation != -1)
	//	{
	//		f->glUniform1i(stageLocation, j);
	//	}
	//
	//	if (pingpongLocation != -1)
	//	{
	//		f->glUniform1i(pingpongLocation, pingpong);
	//	}
	//
	//	if (directionLocation2 != -1)
	//	{
	//		f->glUniform1i(directionLocation2, 1);
	//	}
	//
	//	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
	//	f->glFinish();
	//	pingpong++;
	//	pingpong %= 2;
	//}
	//
	//f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//
	//
	//// inversion
	//GLuint inversion_shader = ShaderManager::GetInstance()->GetShaderProgram("inversion");
	//
	//f->glUseProgram(inversion_shader);
	//glGetError1 = f->glGetError();
	//
	//GLint displacementLocation = f->glGetUniformLocation(inversion_shader, "displacement");
	//GLint pingpong0_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong0");
	//GLint pingpong1_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong1");
	//
	//if (displacementLocation != -1)
	//{
	//	f->glBindImageTexture(0, m_DyTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//}
	//
	//if (pingpong0_Location2 != -1)
	//{
	//	f->glBindImageTexture(1, m_tilde_hkt_dy, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//}
	//
	//if (pingpong1_Location2 != -1)
	//{
	//	f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//}
	//
	//GLint N4Location = f->glGetUniformLocation(inversion_shader, "N");
	//GLint pingpong4Location = f->glGetUniformLocation(inversion_shader, "pingpong");
	//
	//if (N4Location != -1)
	//{
	//	f->glUniform1i(N4Location, m_N);
	//}
	//
	//if (pingpong4Location != -1)
	//{
	//	f->glUniform1i(pingpong4Location, pingpong);
	//}
	//
	//f->glDispatchCompute(m_N / 16, m_N / 16, 1);
	//f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	ComputeButterflyInversion(m_tilde_hkt_dy, m_DyTexture);

	if (m_choppy)
	{
		ComputeButterflyInversion(m_tilde_hkt_dx, m_DxTexture);
		ComputeButterflyInversion(m_tilde_hkt_dz, m_DzTexture);
		//// DX
		//// butterfly
		//f->glUseProgram(butterfly_shader);
		//glGetError1 = f->glGetError();
		//
		//twiddlesIndices_Location = f->glGetUniformLocation(butterfly_shader, "twiddlesIndices");
		//pingpong0_Location = f->glGetUniformLocation(butterfly_shader, "pingpong0");
		//pingpong1_Location = f->glGetUniformLocation(butterfly_shader, "pingpong1");
		//
		//if (twiddlesIndices_Location != -1)
		//{
		//	f->glBindImageTexture(0, m_twiddleIndices, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong0_Location != -1)
		//{
		//	f->glBindImageTexture(1, m_tilde_hkt_dx, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong1_Location != -1)
		//{
		//	f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//stageLocation = f->glGetUniformLocation(butterfly_shader, "stage");
		//pingpongLocation = f->glGetUniformLocation(butterfly_shader, "pingpong");
		//directionLocation2 = f->glGetUniformLocation(butterfly_shader, "direction");
		//
		//// 1D FFT horizontal 
		//pingpong = 0;
		//for (int i = 0; i<m_log_2_N; i++)
		//{
		//	//variables
		//	if (stageLocation != -1)
		//	{
		//		f->glUniform1i(stageLocation, i);
		//	}
		//
		//	if (pingpongLocation != -1)
		//	{
		//		f->glUniform1i(pingpongLocation, pingpong);
		//	}
		//
		//	if (directionLocation2 != -1)
		//	{
		//		f->glUniform1i(directionLocation2, 0);
		//	}
		//
		//	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		//	f->glFinish();
		//	pingpong++;
		//	pingpong %= 2;
		//}
		//
		////1D FFT vertical 
		//for (int j = 0; j<m_log_2_N; j++)
		//{
		//	//variables
		//	if (stageLocation != -1)
		//	{
		//		f->glUniform1i(stageLocation, j);
		//	}
		//
		//	if (pingpongLocation != -1)
		//	{
		//		f->glUniform1i(pingpongLocation, pingpong);
		//	}
		//
		//	if (directionLocation2 != -1)
		//	{
		//		f->glUniform1i(directionLocation2, 1);
		//	}
		//
		//	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		//	f->glFinish();
		//	pingpong++;
		//	pingpong %= 2;
		//}
		//
		//f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//
		//
		//// inversion
		//f->glUseProgram(inversion_shader);
		//glGetError1 = f->glGetError();
		//
		//displacementLocation = f->glGetUniformLocation(inversion_shader, "displacement");
		//pingpong0_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong0");
		//pingpong1_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong1");
		//
		//if (displacementLocation != -1)
		//{
		//	f->glBindImageTexture(0, m_DxTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong0_Location2 != -1)
		//{
		//	f->glBindImageTexture(1, m_tilde_hkt_dx, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong1_Location2 != -1)
		//{
		//	f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//N4Location = f->glGetUniformLocation(inversion_shader, "N");
		//pingpong4Location = f->glGetUniformLocation(inversion_shader, "pingpong");
		//
		//if (N4Location != -1)
		//{
		//	f->glUniform1i(N4Location, m_N);
		//}
		//
		//if (pingpong4Location != -1)
		//{
		//	f->glUniform1i(pingpong4Location, pingpong);
		//}
		//
		//f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		//f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//
		//
		//// DZ
		//// butterfly
		//f->glUseProgram(butterfly_shader);
		//glGetError1 = f->glGetError();
		//
		//twiddlesIndices_Location = f->glGetUniformLocation(butterfly_shader, "twiddlesIndices");
		//pingpong0_Location = f->glGetUniformLocation(butterfly_shader, "pingpong0");
		//pingpong1_Location = f->glGetUniformLocation(butterfly_shader, "pingpong1");
		//
		//if (twiddlesIndices_Location != -1)
		//{
		//	f->glBindImageTexture(0, m_twiddleIndices, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong0_Location != -1)
		//{
		//	f->glBindImageTexture(1, m_tilde_hkt_dz, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong1_Location != -1)
		//{
		//	f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//stageLocation = f->glGetUniformLocation(butterfly_shader, "stage");
		//pingpongLocation = f->glGetUniformLocation(butterfly_shader, "pingpong");
		//directionLocation2 = f->glGetUniformLocation(butterfly_shader, "direction");
		//
		//// 1D FFT horizontal 
		//pingpong = 0;
		//for (int i = 0; i<m_log_2_N; i++)
		//{
		//	//variables
		//	if (stageLocation != -1)
		//	{
		//		f->glUniform1i(stageLocation, i);
		//	}
		//
		//	if (pingpongLocation != -1)
		//	{
		//		f->glUniform1i(pingpongLocation, pingpong);
		//	}
		//
		//	if (directionLocation2 != -1)
		//	{
		//		f->glUniform1i(directionLocation2, 0);
		//	}
		//
		//	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		//	f->glFinish();
		//	pingpong++;
		//	pingpong %= 2;
		//}
		//
		////1D FFT vertical 
		//for (int j = 0; j<m_log_2_N; j++)
		//{
		//	//variables
		//	if (stageLocation != -1)
		//	{
		//		f->glUniform1i(stageLocation, j);
		//	}
		//
		//	if (pingpongLocation != -1)
		//	{
		//		f->glUniform1i(pingpongLocation, pingpong);
		//	}
		//
		//	if (directionLocation2 != -1)
		//	{
		//		f->glUniform1i(directionLocation2, 1);
		//	}
		//
		//	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		//	f->glFinish();
		//	pingpong++;
		//	pingpong %= 2;
		//}
		//
		//f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//
		//
		//// inversion
		//f->glUseProgram(inversion_shader);
		//glGetError1 = f->glGetError();
		//
		//displacementLocation = f->glGetUniformLocation(inversion_shader, "displacement");
		//pingpong0_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong0");
		//pingpong1_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong1");
		//
		//if (displacementLocation != -1)
		//{
		//	f->glBindImageTexture(0, m_DzTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong0_Location2 != -1)
		//{
		//	f->glBindImageTexture(1, m_tilde_hkt_dz, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//if (pingpong1_Location2 != -1)
		//{
		//	f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//}
		//
		//N4Location = f->glGetUniformLocation(inversion_shader, "N");
		//pingpong4Location = f->glGetUniformLocation(inversion_shader, "pingpong");
		//
		//if (N4Location != -1)
		//{
		//	f->glUniform1i(N4Location, m_N);
		//}
		//
		//if (pingpong4Location != -1)
		//{
		//	f->glUniform1i(pingpong4Location, pingpong);
		//}
		//
		//f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		//f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	// Draw Water
	
	GLuint waterifftShader = ShaderManager::GetInstance()->GetShaderProgram("waterifft");
	glGetError1 = f->glGetError();

	// DRAW TERRAIN PATCHES
	f->glUseProgram(waterifftShader);
	glGetError1 = f->glGetError();


	GLint heightMapLocation = f->glGetUniformLocation(waterifftShader, "displ_y");

	if (heightMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0);
		f->glActiveTexture(GL_TEXTURE0);
		f->glBindTexture(GL_TEXTURE_2D, m_DyTexture);
		f->glUniform1i(heightMapLocation, 0);
	}

	if (m_choppy)
	{
		GLint dispXLocation = f->glGetUniformLocation(waterifftShader, "displ_x");
		if (dispXLocation != -1)
		{
			f->glEnable(GL_TEXTURE0 + 1);
			f->glActiveTexture(GL_TEXTURE0 + 1);
			f->glBindTexture(GL_TEXTURE_2D, m_DxTexture);
			f->glUniform1i(dispXLocation, 1);
		}

		GLint dispZLocation = f->glGetUniformLocation(waterifftShader, "displ_z");
		if (dispZLocation != -1)
		{
			f->glEnable(GL_TEXTURE0 + 2);
			f->glActiveTexture(GL_TEXTURE0 + 2);
			f->glBindTexture(GL_TEXTURE_2D, m_DzTexture);
			f->glUniform1i(dispZLocation, 2);
		}
	}


	f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	f->glBufferData(GL_ARRAY_BUFFER, m_patch_vert.size() * sizeof(GLfloat), m_patch_vert.data(), GL_STATIC_DRAW);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(0);

	GLint mvpLocation = f->glGetUniformLocation(waterifftShader, "mvp");
	if (mvpLocation != -1)
	{
		f->glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	GLint choppyLocation = f->glGetUniformLocation(waterifftShader, "choppy");
	if (choppyLocation != -1)
	{
		float val = m_choppy ? 1.0f : -1.0f;
		f->glUniform1f(choppyLocation, val);
	}

	GLint screenResolutionLocation = f->glGetUniformLocation(waterifftShader, "screenResolution");
	if (screenResolutionLocation != -1)
	{
		f->glUniform2fv(screenResolutionLocation, 1, glm::value_ptr(m_screenResolution));
	}

	//
	GLint cameraPosLocation = f->glGetUniformLocation(waterifftShader, "cameraPos");
	if (cameraPosLocation != -1)
	{
		f->glUniform3fv(cameraPosLocation, 1, glm::value_ptr(cameraPos));
	}

	float patchSize = 16;
	GLint patchSizeLocation = f->glGetUniformLocation(waterifftShader, "patchSize");
	if (patchSizeLocation != -1)
	{
		f->glUniform1f(patchSizeLocation, patchSize);
	}

	int gridSize = 8;
	GLint gridSizeLocation = f->glGetUniformLocation(waterifftShader, "gridSize");
	if (gridSizeLocation != -1)
	{
		f->glUniform1i(gridSizeLocation, gridSize);
	}

	GLint debugModeLocation = f->glGetUniformLocation(waterifftShader, "debug_mode_enabled");
	if (debugModeLocation != -1)
	{
		int debugVal = debugMode ? 1 : 0;
		f->glUniform1i(debugModeLocation, debugVal);
	}

	f->glEnable(GL_DEPTH_TEST);
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);

	if (debugMode)
	{
		f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	f->glPatchParameteri(GL_PATCH_VERTICES, 4);
	f->glDrawArraysInstanced(GL_PATCHES, 0, 4, gridSize * gridSize);

	f->glDisable(GL_CULL_FACE);
	f->glDisable(GL_DEPTH_TEST);


}
////////////////////////////////////////////////////////////////////////
void WaterIFFT::ComputeButterflyInversion(GLuint tildehtkTexture, GLuint DisplTexture)
{
	// DZ
	// butterfly
	GLuint butterfly_shader = ShaderManager::GetInstance()->GetShaderProgram("butterfly");
	f->glUseProgram(butterfly_shader);
	GLenum glGetError1 = f->glGetError();

	GLint twiddlesIndices_Location = f->glGetUniformLocation(butterfly_shader, "twiddlesIndices");
	GLint pingpong0_Location = f->glGetUniformLocation(butterfly_shader, "pingpong0");
	GLint pingpong1_Location = f->glGetUniformLocation(butterfly_shader, "pingpong1");

	if (twiddlesIndices_Location != -1)
	{
		f->glBindImageTexture(0, m_twiddleIndices, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	if (pingpong0_Location != -1)
	{
		f->glBindImageTexture(1, tildehtkTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	if (pingpong1_Location != -1)
	{
		f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	GLint stageLocation = f->glGetUniformLocation(butterfly_shader, "stage");
	GLint pingpongLocation = f->glGetUniformLocation(butterfly_shader, "pingpong");
	GLint directionLocation2 = f->glGetUniformLocation(butterfly_shader, "direction");

	// 1D FFT horizontal 
	pingpong = 0;
	for (int i = 0; i<m_log_2_N; i++)
	{
		//variables
		if (stageLocation != -1)
		{
			f->glUniform1i(stageLocation, i);
		}

		if (pingpongLocation != -1)
		{
			f->glUniform1i(pingpongLocation, pingpong);
		}

		if (directionLocation2 != -1)
		{
			f->glUniform1i(directionLocation2, 0);
		}

		f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		f->glFinish();
		pingpong++;
		pingpong %= 2;
	}

	//1D FFT vertical 
	for (int j = 0; j<m_log_2_N; j++)
	{
		//variables
		if (stageLocation != -1)
		{
			f->glUniform1i(stageLocation, j);
		}

		if (pingpongLocation != -1)
		{
			f->glUniform1i(pingpongLocation, pingpong);
		}

		if (directionLocation2 != -1)
		{
			f->glUniform1i(directionLocation2, 1);
		}

		f->glDispatchCompute(m_N / 16, m_N / 16, 1);
		f->glFinish();
		pingpong++;
		pingpong %= 2;
	}

	f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	// inversion
	GLuint inversion_shader = ShaderManager::GetInstance()->GetShaderProgram("inversion");
	f->glUseProgram(inversion_shader);
	glGetError1 = f->glGetError();

	GLint displacementLocation = f->glGetUniformLocation(inversion_shader, "displacement");
	GLint pingpong0_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong0");
	GLint pingpong1_Location2 = f->glGetUniformLocation(inversion_shader, "pingpong1");

	if (displacementLocation != -1)
	{
		f->glBindImageTexture(0, DisplTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	if (pingpong0_Location2 != -1)
	{
		f->glBindImageTexture(1, tildehtkTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	if (pingpong1_Location2 != -1)
	{
		f->glBindImageTexture(2, m_pingpong0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	GLint N4Location = f->glGetUniformLocation(inversion_shader, "N");
	GLint pingpong4Location = f->glGetUniformLocation(inversion_shader, "pingpong");

	if (N4Location != -1)
	{
		f->glUniform1i(N4Location, m_N);
	}

	if (pingpong4Location != -1)
	{
		f->glUniform1i(pingpong4Location, pingpong);
	}

	f->glDispatchCompute(m_N / 16, m_N / 16, 1);
	f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
////////////////////////////////////////////////////////////////////////
std::vector<int> WaterIFFT::InitBitReversedIndices()
{
	int N = 512;
	std::vector<int> bitReversedIndices(N, 0);
	
	int bits = (int)(log(N) / log(2));
	
	for (int i = 0; i<N; i++)
	{
		unsigned int x = reverseBits(i);
		x = (x << bits) | (x >> ( -bits & 31 ));
		bitReversedIndices[i] = x;
	}

	return bitReversedIndices;
}
////////////////////////////////////////////////////////////////////////
// function to reverse bits of a number
unsigned int WaterIFFT::reverseBits(unsigned int n)
{
	unsigned int rev = 0;
	unsigned int pow = 31;
	unsigned int one = 1;
	// traversing bits of 'n' from the right
	while (n > 0)
	{
		if(n & 1)
			rev |= one << pow;
		pow --;
		n >>= 1;
	}

	// required number
	return rev;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

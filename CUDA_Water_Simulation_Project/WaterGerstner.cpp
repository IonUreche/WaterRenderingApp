#include "WaterGerstner.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"

#include "TextureManager.h"

WaterGerstner::WaterGerstner(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
////////////////////////////////////////////////////////////////////////
WaterGerstner::~WaterGerstner()
{
}
////////////////////////////////////////////////////////////////////////
void WaterGerstner::InitGeometry()
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
}
////////////////////////////////////////////////////////////////////////
void WaterGerstner::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
}
////////////////////////////////////////////////////////////////////////
void WaterGerstner::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos, bool debugMode)
{
	m_time += 0.1f;
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	//m_lightPosition.x = std::sin(m_time * 20) * 100;
	//m_lightPosition.z = std::cos(m_time * 20) * 100;
	//m_lightPosition.y = 50 + std::cos(m_time * 20) * 50;

	GLuint waterflowShader = ShaderManager::GetInstance()->GetShaderProgram("waterflow");
	GLuint dudvmapTexture = TextureManager::GetInstance()->GetTexture("waterDUDV");
	GLuint normalMapTexture = TextureManager::GetInstance()->GetTexture("matchingNormalMap");
	
	GLenum glGetError1 = f->glGetError();

	// DRAW TERRAIN PATCHES
	f->glUseProgram(waterflowShader);
	glGetError1 = f->glGetError();

	GLint reflLocation = f->glGetUniformLocation(waterflowShader, "reflectionTexture");
	glGetError1 = f->glGetError();
	GLint refrLocation = f->glGetUniformLocation(waterflowShader, "refractionTexture");
	glGetError1 = f->glGetError();

	// Texture unit 0 is for base images.
	//glUniform1i(grassLocation, 2); // Texture unit 2 is for normal maps.
	//glUniform1i(rockLocation, 3); // Texture unit 4 is for shadow maps.
	//glEnable(GL_TEXTURE4);
	//glEnable(GL_TEXTURE1);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	//glBindSampler(0, linearFiltering);

	//GLint heightMapLocation = f->glGetUniformLocation(terrainShader, "hhh");
	GLint heightMap2Location = f->glGetUniformLocation(waterflowShader, "heightMap2");
	GLint dudvMapLocation = f->glGetUniformLocation(waterflowShader, "dudvMap");
	GLint normalMapLocation = f->glGetUniformLocation(waterflowShader, "normalMap");
	GLint depthMapLocation = f->glGetUniformLocation(waterflowShader, "depthMap");
	GLint cubeMapLocation = f->glGetUniformLocation(waterflowShader, "skyboxTexture");
	//glActiveTexture(GL_TEXTURE0 + 4);
	//glBindTexture(GL_TEXTURE_2D, grassTexture);
	//glBindSampler(1, linearFiltering);

	//if (refrLocation != -1)
	//{
	//	f->glEnable(GL_TEXTURE0 + 1);
	//	f->glActiveTexture(GL_TEXTURE0 + 1);
	//	f->glBindTexture(GL_TEXTURE_2D, m_colorTex0);
	//	f->glUniform1i(refrLocation, 1);
	//}

	if (reflLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 2);
		f->glActiveTexture(GL_TEXTURE0 + 2);
		f->glBindTexture(GL_TEXTURE_2D, m_colorTex1);
		f->glUniform1i(reflLocation, 2);
	}

	GLuint modelViewLocation = f->glGetUniformLocation(waterflowShader, "modelView_matrix");
	if(modelViewLocation) 
	{
		f->glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(m_cameraProjection));
	}
	if (dudvMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 4);
		f->glActiveTexture(GL_TEXTURE0 + 4);
		f->glBindTexture(GL_TEXTURE_2D, dudvmapTexture);
		f->glUniform1i(dudvMapLocation, 4);
	}
	//
	if (normalMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 1);
		f->glActiveTexture(GL_TEXTURE0 + 1);
		f->glBindTexture(GL_TEXTURE_2D, normalMapTexture);
		f->glUniform1i(normalMapLocation, 1);
	}

	//if (depthMapLocation != -1)
	//{
	//	f->glEnable(GL_TEXTURE0 + 5);
	//	f->glActiveTexture(GL_TEXTURE0 + 5);
	//	f->glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthTex);
	//	f->glUniform1i(depthMapLocation, 5);
	//}

	if (cubeMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 3);
		f->glActiveTexture(GL_TEXTURE0 + 3);
		auto skyboxTexture = TextureManager::GetInstance()->GetTexture("cubemap");
		f->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		f->glUniform1i(cubeMapLocation, 3);
	}

	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	//glUniform1i(glGetUniformLocation(m_ShaderProgramId, "skybox"), 0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);

	GLuint wavesDataLocation = f->glGetUniformLocation(waterflowShader, "waves");
	if (wavesDataLocation != -1)
	{
		f->glUniform1fv(wavesDataLocation, m_waterParams.size() * sizeof(float), (const GLfloat *)m_waterParams.data());
	}

	GLuint wavesNMDataLocation = f->glGetUniformLocation(waterflowShader, "waves_nm");
	if (wavesNMDataLocation != -1)
	{
		f->glUniform1fv(wavesNMDataLocation, m_waterNormalsParams.size() * sizeof(float), (const GLfloat *)m_waterNormalsParams.data());
	}

	//if (noiseLocation != -1)
	//{
	//	f->glEnable(GL_TEXTURE0 + 3);
	//	f->glActiveTexture(GL_TEXTURE0 + 3);
	//	f->glBindTexture(GL_TEXTURE_2D, noiseTexture);
	//	f->glUniform1i(rockLocation, 3);
	//}
	//
	//if (heightMapLocation != -1)
	//{
	//	f->glEnable(GL_TEXTURE0 + 5);
	//	f->glActiveTexture(GL_TEXTURE0 + 5);
	//	f->glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	//	f->glUniform1i(heightMapLocation, 5);
	//}

	//glActiveTexture(GL_TEXTURE0 + 3);
	//glBindTexture(GL_TEXTURE_2D, grassTexture);
	//glUniform1i(heightMap2Location, 3);

	f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	f->glBufferData(GL_ARRAY_BUFFER, m_patch_vert.size() * sizeof(GLfloat), m_patch_vert.data(), GL_STATIC_DRAW);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	f->glEnableVertexAttribArray(0);

	GLint mvpLocation = f->glGetUniformLocation(waterflowShader, "mvp");
	if (mvpLocation != -1)
	{
		f->glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	GLint viewLocation = f->glGetUniformLocation(waterflowShader, "view");
	if (viewLocation != -1)
	{
		f->glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(m_view));
	}

	//
	//GLuint modelViewLocation = glGetUniformLocation(m_terrainShader, "modelView_matrix");
	//glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(camera.projection));
	//
	GLint timeLocation = f->glGetUniformLocation(waterflowShader, "time");
	if (timeLocation != -1)
	{
		f->glUniform1f(timeLocation, m_time);
	}

	GLint moveFactorLocation = f->glGetUniformLocation(waterflowShader, "moveFactor");
	if (moveFactorLocation != -1)
	{
		f->glUniform1f(moveFactorLocation, m_time * 0.01);
	}
	//
	//GLuint debugModeLocation = glGetUniformLocation(m_terrainShader, "debug_mode_enabled");
	//glUniform1i(debugModeLocation, m_debugModeView);
	//
	GLint lightPosLocation = f->glGetUniformLocation(waterflowShader, "lightPos");
	if (lightPosLocation != -1)
	{
		f->glUniform3fv(lightPosLocation, 1, glm::value_ptr(m_lightPosition));
	}

	GLint screenResolutionLocation = f->glGetUniformLocation(waterflowShader, "screenResolution");
	if (screenResolutionLocation != -1)
	{
		f->glUniform2fv(screenResolutionLocation, 1, glm::value_ptr(m_screenResolution));
	}
	
	//
	GLint cameraPosLocation = f->glGetUniformLocation(waterflowShader, "cameraPos");
	if (cameraPosLocation != -1)
	{
		f->glUniform3fv(cameraPosLocation, 1, glm::value_ptr(cameraPos));
	}
	//
	//GLuint wavesDataLocation = glGetUniformLocation(m_terrainShader, "waves");
	//glUniform1fv(wavesDataLocation, 24 * sizeof(float), (const GLfloat *)m_wavesGeometricData.data());

	float patchSize = 16;
	GLint patchSizeLocation = f->glGetUniformLocation(waterflowShader, "patchSize");
	if (patchSizeLocation != -1)
	{
		f->glUniform1f(patchSizeLocation, patchSize);
	}

	int gridSize = 8;
	GLint gridSizeLocation = f->glGetUniformLocation(waterflowShader, "gridSize");
	if (gridSizeLocation != -1)
	{
		f->glUniform1i(gridSizeLocation, gridSize);
	}

	GLint debugModeLocation = f->glGetUniformLocation(waterflowShader, "debug_mode_enabled");
	if (debugModeLocation != -1)
	{
		int debugVal = debugMode ? 1 : 0;
		f->glUniform1i(debugModeLocation, debugVal);
	}
	
	//
	//glEnable(GL_TEXTURE0);
	//
	//m_waterColor->bind(1);
	//m_waterNormal->bind(2);
	//m_skyboxTex->bind(3);
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
////////////////////////////////////////////////////////////////////////

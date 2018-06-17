#include "Water.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"

#include "TextureManager.h"

Water::Water(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
////////////////////////////////////////////////////////////////////////
Water::~Water()
{
}
////////////////////////////////////////////////////////////////////////
void Water::InitGeometry()
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
void Water::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
}
////////////////////////////////////////////////////////////////////////
void Water::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos)
{
	m_time += 0.001f;
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	//m_lightPosition.x = std::sin(m_time * 20) * 100;
	//m_lightPosition.z = std::cos(m_time * 20) * 100;
	//m_lightPosition.y = 50 + std::cos(m_time * 20) * 50;

	GLuint terrainShader = ShaderManager::GetInstance()->GetShaderProgram("water");
	//GLuint grassTexture = TextureManager::GetInstance()->GetTexture("grass");
	//GLuint rockTexture = TextureManager::GetInstance()->GetTexture("rockStones");
	//GLuint heightMapTexture = TextureManager::GetInstance()->GetTexture("heightmap0_512");
	//GLuint noiseTexture = TextureManager::GetInstance()->GetTexture("Noise512");
	GLuint dudvmapTexture = TextureManager::GetInstance()->GetTexture("waterDUDV");
	//GLuint dudvmapTexture2 = TextureManager::GetInstance()->GetTexture("dudv0");
	//GLuint dudvmapTexture3 = TextureManager::GetInstance()->GetTexture("dudv_fbo");
	GLuint normalMapTexture = TextureManager::GetInstance()->GetTexture("matchingNormalMap");
	

	GLenum glGetError1 = f->glGetError();

	// DRAW TERRAIN PATCHES
	f->glUseProgram(terrainShader);
	glGetError1 = f->glGetError();

	GLint planeLocation = f->glGetUniformLocation(terrainShader, "plane");
	glGetError1 = f->glGetError();
	if (planeLocation != -1)
	{
		f->glUniform4fv(planeLocation, 1, glm::value_ptr(m_plane));
	}

	//// bind the framebuffer as the output framebuffer
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//
	//// define the index array for the outputs
	//GLuint attachments[2] = { GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT/*, GL_COLOR_ATTACHMENT2*/ };
	//glDrawBuffers(2, attachments);
	//
	//glBindVertexArray(m_vao);
	//

	//GLint grassLocation = f->glGetUniformLocation(terrainShader, "grass");
	//glGetError1 = glGetError();
	GLint reflLocation = f->glGetUniformLocation(terrainShader, "reflectionTexture");
	glGetError1 = f->glGetError();
	GLint refrLocation = f->glGetUniformLocation(terrainShader, "refractionTexture");
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
	GLint heightMap2Location = f->glGetUniformLocation(terrainShader, "heightMap2");
	GLint dudvMapLocation = f->glGetUniformLocation(terrainShader, "dudvMap");
	GLint normalMapLocation = f->glGetUniformLocation(terrainShader, "normalMap");
	GLint depthMapLocation = f->glGetUniformLocation(terrainShader, "depthMap");

	//glActiveTexture(GL_TEXTURE0 + 4);
	//glBindTexture(GL_TEXTURE_2D, grassTexture);
	//glBindSampler(1, linearFiltering);

	if (refrLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 1);
		f->glActiveTexture(GL_TEXTURE0 + 1);
		f->glBindTexture(GL_TEXTURE_2D, m_colorTex0);
		f->glUniform1i(refrLocation, 1);
	}

	if (reflLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 2);
		f->glActiveTexture(GL_TEXTURE0 + 2);
		f->glBindTexture(GL_TEXTURE_2D, m_colorTex1);
		f->glUniform1i(reflLocation, 2);
	}

	if (dudvMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 3);
		f->glActiveTexture(GL_TEXTURE0 + 3);
		f->glBindTexture(GL_TEXTURE_2D, dudvmapTexture);
		f->glUniform1i(dudvMapLocation, 3);
	}

	if (normalMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 4);
		f->glActiveTexture(GL_TEXTURE0 + 4);
		f->glBindTexture(GL_TEXTURE_2D, normalMapTexture);
		f->glUniform1i(normalMapLocation, 4);
	}

	if (depthMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 5);
		f->glActiveTexture(GL_TEXTURE0 + 5);
		f->glBindTexture(GL_TEXTURE_2D, m_depthTex);
		f->glUniform1i(depthMapLocation, 5);
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

	GLint mvpLocation = f->glGetUniformLocation(terrainShader, "mvp");
	if (mvpLocation != -1)
	{
		f->glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
	}

	GLint viewLocation = f->glGetUniformLocation(terrainShader, "view");
	if (viewLocation != -1)
	{
		f->glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(m_view));
	}

	//
	//GLuint modelViewLocation = glGetUniformLocation(m_terrainShader, "modelView_matrix");
	//glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(camera.projection));
	//
	GLint moveFactorLocation = f->glGetUniformLocation(terrainShader, "moveFactor");
	if (moveFactorLocation != -1)
	{
		f->glUniform1f(moveFactorLocation, m_time);
	}
	//
	//GLuint debugModeLocation = glGetUniformLocation(m_terrainShader, "debug_mode_enabled");
	//glUniform1i(debugModeLocation, m_debugModeView);
	//
	GLint lightPosLocation = f->glGetUniformLocation(terrainShader, "lightPos");
	if (lightPosLocation != -1)
	{
		f->glUniform3fv(lightPosLocation, 1, glm::value_ptr(m_lightPosition));
	}

	GLint screenResolutionLocation = f->glGetUniformLocation(terrainShader, "screenResolution");
	if (screenResolutionLocation != -1)
	{
		f->glUniform2fv(screenResolutionLocation, 1, glm::value_ptr(m_screenResolution));
	}
	
	//
	GLint cameraPosLocation = f->glGetUniformLocation(terrainShader, "cameraPos");
	if (cameraPosLocation != -1)
	{
		f->glUniform3fv(cameraPosLocation, 1, glm::value_ptr(cameraPos));
	}
	//
	//GLuint wavesDataLocation = glGetUniformLocation(m_terrainShader, "waves");
	//glUniform1fv(wavesDataLocation, 24 * sizeof(float), (const GLfloat *)m_wavesGeometricData.data());

	float patchSize = 16;
	GLint patchSizeLocation = f->glGetUniformLocation(terrainShader, "patchSize");
	if (patchSizeLocation != -1)
	{
		f->glUniform1f(patchSizeLocation, patchSize);
	}

	int gridSize = 8;
	GLint gridSizeLocation = f->glGetUniformLocation(terrainShader, "gridSize");
	if (gridSizeLocation != -1)
	{
		f->glUniform1i(gridSizeLocation, gridSize);
	}
	//
	//glEnable(GL_TEXTURE0);
	//
	//m_waterColor->bind(1);
	//m_waterNormal->bind(2);
	//m_skyboxTex->bind(3);
	//
	//if (m_isWireframeMode)
	//{
	//f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//}
	//else
	//{
	f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//}
	f->glPatchParameteri(GL_PATCH_VERTICES, 4);
	f->glDrawArraysInstanced(GL_PATCHES, 0, 4, gridSize * gridSize);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

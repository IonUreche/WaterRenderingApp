#include "Terrain.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"


#include "TextureManager.h"

Terrain::Terrain(QOpenGLContext *glContext, GLuint fbo)
{
	m_fbo = fbo;
	m_pGLContext = glContext;
	f = glContext->versionFunctions<QOpenGLFunctions_4_5_Core>();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
Terrain::~Terrain()
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Terrain::InitGeometry()
{
	// init patch vertices
	m_patch_vert =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Terrain::InitBuffers()
{
	f->glGenBuffers(1, &m_vbo);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Terrain::Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos)
{
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	// define the index array for the outputs
	//GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT/*, GL_COLOR_ATTACHMENT2*/ };
	//f->glDrawBuffers(2, attachments);
	//f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	GLuint terrainShader = ShaderManager::GetInstance()->GetShaderProgram("terrain");

	GLuint grassTexture = TextureManager::GetInstance()->GetTexture("grass");
	GLuint rockTexture = TextureManager::GetInstance()->GetTexture("rockStones");
	GLuint heightMapTexture = TextureManager::GetInstance()->GetTexture("test");
	GLuint noiseTexture = TextureManager::GetInstance()->GetTexture("Noise512");

	GLenum glGetError1 = f->glGetError();

	// DRAW TERRAIN PATCHES
	f->glUseProgram(terrainShader);
	glGetError1 = f->glGetError();

	//// bind the framebuffer as the output framebuffer
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//
	//// define the index array for the outputs
	//GLuint attachments[2] = { GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT/*, GL_COLOR_ATTACHMENT2*/ };
	//glDrawBuffers(2, attachments);
	//
	//glBindVertexArray(m_vao);
	//

	GLint planeLocation = f->glGetUniformLocation(terrainShader, "plane");
	glGetError1 = f->glGetError();
	if (planeLocation != -1)
	{
		f->glUniform4fv(planeLocation, 1, glm::value_ptr(m_plane));
	}

	GLint grassLocation = f->glGetUniformLocation(terrainShader, "grass");
	glGetError1 = f->glGetError();
	GLint rockLocation = f->glGetUniformLocation(terrainShader, "rock");
	glGetError1 = f->glGetError();
	GLint noiseLocation = f->glGetUniformLocation(terrainShader, "noise");
	glGetError1 = f->glGetError();

	// Texture unit 0 is for base images.
	//glUniform1i(grassLocation, 2); // Texture unit 2 is for normal maps.
	//glUniform1i(rockLocation, 3); // Texture unit 4 is for shadow maps.
	//glEnable(GL_TEXTURE4);
	//glEnable(GL_TEXTURE1);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	//glBindSampler(0, linearFiltering);

	GLint heightMapLocation = f->glGetUniformLocation(terrainShader, "hhh");
	GLint heightMap2Location = f->glGetUniformLocation(terrainShader, "heightMap2");
	
	//glActiveTexture(GL_TEXTURE0 + 4);
	//glBindTexture(GL_TEXTURE_2D, grassTexture);
	//glBindSampler(1, linearFiltering);

	if (grassLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 2);
		f->glActiveTexture(GL_TEXTURE0 + 2);
		f->glBindTexture(GL_TEXTURE_2D, grassTexture);
		f->glUniform1i(grassLocation, 2);
	}
	
	if (rockLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 1);
		f->glActiveTexture(GL_TEXTURE0 + 1);
		f->glBindTexture(GL_TEXTURE_2D, rockTexture);
		f->glUniform1i(rockLocation, 1);
	}

	if (noiseLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 3);
		f->glActiveTexture(GL_TEXTURE0 + 3);
		f->glBindTexture(GL_TEXTURE_2D, noiseTexture);
		f->glUniform1i(rockLocation, 3);
	}

	if (heightMapLocation != -1)
	{
		f->glEnable(GL_TEXTURE0 + 5);
		f->glActiveTexture(GL_TEXTURE0 + 5);
		f->glBindTexture(GL_TEXTURE_2D, heightMapTexture);
		f->glUniform1i(heightMapLocation, 5);
	}

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
	
	//
	//GLuint modelViewLocation = glGetUniformLocation(m_terrainShader, "modelView_matrix");
	//glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(camera.projection));
	//
	//GLuint timeLocation = glGetUniformLocation(m_terrainShader, "time");
	//glUniform1f(timeLocation, m_time);
	//
	//GLuint debugModeLocation = glGetUniformLocation(m_terrainShader, "debug_mode_enabled");
	//glUniform1i(debugModeLocation, m_debugModeView);
	//
	//GLuint lightPosLocation = glGetUniformLocation(m_terrainShader, "lightPos");
	//glUniform3fv(lightPosLocation, 1, glm::value_ptr(m_lightPosition));
	//
	//GLint cameraPosLocation = f->glGetUniformLocation(terrainShader, "cameraPos");
	//f->glUniform3fv(cameraPosLocation, 1, glm::value_ptr(cameraPos));
	//
	//GLuint wavesDataLocation = glGetUniformLocation(m_terrainShader, "waves");
	//glUniform1fv(wavesDataLocation, 24 * sizeof(float), (const GLfloat *)m_wavesGeometricData.data());

	int patchSize = 32;
	GLint patchSizeLocation = f->glGetUniformLocation(terrainShader, "patchSize");
	if (patchSizeLocation != -1)
	{
		f->glUniform1i(patchSizeLocation, patchSize);
	}

	int gridSize = 4;
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
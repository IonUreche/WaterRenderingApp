#include "Terrain.h"



Terrain::Terrain()
{
	initializeOpenGLFunctions();
}


Terrain::~Terrain()
{
}

void Terrain::Draw()
{
	GLuint terrainShader = ShaderManager::GetInstance()->GetShaderProgram("terrain");

	// DRAW TERRAIN PATCHES
	glUseProgram(terrainShader);

	//// bind the framebuffer as the output framebuffer
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//
	//// define the index array for the outputs
	//GLuint attachments[2] = { GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT/*, GL_COLOR_ATTACHMENT2*/ };
	//glDrawBuffers(2, attachments);
	//
	//glBindVertexArray(m_vao);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferData(GL_ARRAY_BUFFER, m_patch_vert.size() * sizeof(GLfloat), m_patch_vert.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);
	//
	//GLuint mvpLocation = glGetUniformLocation(m_terrainShader, "mvp");
	//glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(m_mvp));
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
	//GLuint cameraPosLocation = glGetUniformLocation(m_terrainShader, "cameraPos");
	//glUniform3fv(cameraPosLocation, 1, glm::value_ptr(camera.camera_position));
	//
	//GLuint wavesDataLocation = glGetUniformLocation(m_terrainShader, "waves");
	//glUniform1fv(wavesDataLocation, 24 * sizeof(float), (const GLfloat *)m_wavesGeometricData.data());
	//
	//glEnable(GL_TEXTURE0);
	//
	//m_waterColor->bind(1);
	//m_waterNormal->bind(2);
	//m_skyboxTex->bind(3);
	//
	//if (m_isWireframeMode)
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//}
	//else
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//}
	//glPatchParameteri(GL_PATCH_VERTICES, 4);
	//glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
}
#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include "ShaderManager.h"
#include "glm/gtc/type_ptr.hpp"

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLContext>

class IDrawable
{
public:
	virtual void InitBuffers() = 0;
	virtual void InitGeometry() = 0;
	virtual void Draw(glm::mat4x4 &mvp, glm::vec3 cameraPos) = 0;
	QOpenGLContext* m_pGLContext = nullptr;
	QOpenGLFunctions_4_5_Core* f = nullptr;
	GLuint m_fbo;
};


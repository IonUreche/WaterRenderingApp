#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include "ShaderManager.h"

class IDrawable : protected QOpenGLFunctions_4_5_Core
{
public:
	virtual void Draw() = 0;
};


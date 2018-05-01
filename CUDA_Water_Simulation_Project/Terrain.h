#pragma once
#include "IDrawable.h"

class Terrain : public IDrawable 
{
public:
	Terrain();
	~Terrain();

	void Draw();
};


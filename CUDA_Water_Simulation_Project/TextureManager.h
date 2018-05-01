#pragma once

#include <QOpenGLFunctions_4_5_Core>

#include <unordered_map>

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	uint32_t LoadTexture(const char* textureFilePath, int format);
	uint32_t GetTexture(std::string textureName);
	void ReleaseTexture(std::string textureName);

private:
	std::unordered_map<std::string, uint32_t> m_textures;
};


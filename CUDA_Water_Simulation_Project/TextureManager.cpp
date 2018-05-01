#include "TextureManager.h"
#include "stb_image.h"

TextureManager::TextureManager()
{
	std::string s = "src/Resources/Water/Water0normal.png";
	auto x = LoadTexture(s.c_str(), GL_RGB);

	size_t lastPos = -1;
	lastPos = s.find_last_of("/");
	std::string FileName = s.substr(lastPos + 1);
	m_textures[FileName] = x;
}
//////////////////////////////////////////////////////////////////////////////
TextureManager::~TextureManager()
{
}
//////////////////////////////////////////////////////////////////////////////
GLuint TextureManager::LoadTexture(const char* textureFilePath, int format)
{
	FILE *f = fopen(textureFilePath, "r");
	int w, h, c;
	auto b = stbi_load_from_file(f, &w, &h, &c, 0);
	int ff;
	if (format == GL_RGB)
	{
		ff = STBI_rgb;
	}
	else
	if (format == GL_RGBA)
	{
		ff = STBI_rgb_alpha;
	}
	else
	{
		throw(std::string("Failed to load texture: format error is wrong"));
		return -1;
	}

	unsigned char* image = stbi_load(textureFilePath, &w, &h, &c, ff);

	if (image == nullptr)
		throw(std::string("Failed to load texture"));

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	fclose(f);

	return texture;
}
//////////////////////////////////////////////////////////////////////////////
uint32_t TextureManager::GetTexture(std::string textureName)
{
	return -1;
}
//////////////////////////////////////////////////////////////////////////////
void TextureManager::ReleaseTexture(std::string textureName)
{
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
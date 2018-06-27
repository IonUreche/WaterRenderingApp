#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"

TextureManager::TextureManager()
{
}
//////////////////////////////////////////////////////////////////////////////
void TextureManager::Init()
{
	LoadTextureImage("src/Resources/Water/waterDUDV.png", GL_RGB);
	LoadTextureImage("src/Resources/Water/dudv0.jpg", GL_RGB);
	LoadTextureImage("src/Resources/Water/dudv_fbo.png", GL_RGB);
	LoadTextureImage("src/Resources/Water/matchingNormalMap.png", GL_RGB);

	LoadTextureImage("src/Resources/Terrain/grass.jpg", GL_RGB);
	LoadTextureImage("src/Resources/Terrain/rockStones.jpg", GL_RGB);
	LoadTextureImage("src/Resources/Terrain/heightmap0_512.png", GL_RGB);
	LoadTextureImage("src/Resources/Terrain/Noise512.png", GL_RGB);
	LoadTextureImage("src/Resources/Terrain/test.png", GL_RGB);
	LoadTextureImage("src/Resources/Terrain/test2.png", GL_RGB);

	LoadTextureImage("src/Resources/Noise/Noise512_0.jpg", GL_RGB);
	LoadTextureImage("src/Resources/Noise/Noise512_1.jpg", GL_RGB);
	LoadTextureImage("src/Resources/Noise/Noise512_2.jpg", GL_RGB);
	LoadTextureImage("src/Resources/Noise/Noise512_3.jpg", GL_RGB);

	std::vector<std::string> faces = {
		"src/Resources/Skyboxes/12/test/sky12_RT.jpg",
		"src/Resources/Skyboxes/12/test/sky12_LF.jpg",
		"src/Resources/Skyboxes/12/test/sky12_UP.jpg",
		"src/Resources/Skyboxes/12/test/sky12_DN.jpg",
		"src/Resources/Skyboxes/12/test/sky12_BK.jpg",
		"src/Resources/Skyboxes/12/test/sky12_FR.jpg"
	};
	auto x = LoadCubemapTexture(faces, GL_RGB);
	std::string FileName = "cubemap";
	if (x)
	{
		auto it = std::find_if(m_textures.begin(), m_textures.end(), [FileName](auto entry)->bool { return entry.first == FileName; });
		if (it == m_textures.end())
		{
			m_textures[FileName] = x;
		}
		else
		{
			assert(0 && "Duplicate Texture");
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
TextureManager::~TextureManager()
{
	if(m_pInstance)
		delete m_pInstance;
}
//////////////////////////////////////////////////////////////////////////////
TextureManager* TextureManager::GetInstance()
{
	if (!m_pInstance)
		m_pInstance = new TextureManager();
	return m_pInstance; 
}
//////////////////////////////////////////////////////////////////////////////
GLuint TextureManager::LoadTextureImage(std::string textureFilePath, int format)
{
	GLuint x = LoadTexture(textureFilePath.c_str(), format);

	size_t lastPos = -1;
	lastPos = textureFilePath.find_last_of("/");
	std::string FileName = textureFilePath.substr(lastPos + 1);
	size_t lastDotPos = -1;
	lastDotPos = FileName.find_last_of(".");
	FileName = FileName.substr(0, lastDotPos);

	if (x)
	{
		auto it = std::find_if(m_textures.begin(), m_textures.end(), [FileName](auto entry)->bool { return entry.first == FileName; });
		if (it == m_textures.end())
		{
			m_textures[FileName] = x;
		}
		else
		{
			assert(0 && "Duplicate Texture");
		}
		return x;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
GLuint TextureManager::LoadTexture(const char* textureFilePath, int format)
{
	FILE *f = fopen(textureFilePath, "r");
	int w, h, c;
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

	m_f->glGenTextures(1, &texture);
	m_f->glBindTexture(GL_TEXTURE_2D, texture);
	m_f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_f->glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
	m_f->glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	fclose(f);

	return texture;
}
//////////////////////////////////////////////////////////////////////////////
GLuint TextureManager::LoadCubemapTexture(std::vector<std::string> faces, int format)
{
	GLuint texture;
	m_f->glGenTextures(1, &texture);
	m_f->glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (int i = 0; i < (int)faces.size(); ++i)
	{
		FILE *f = fopen(faces[i].c_str(), "r");
		int w, h, c;
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

		unsigned char* image = stbi_load(faces[i].c_str(), &w, &h, &c, ff);

		if (image == nullptr)
			throw(std::string("Failed to load texture"));

		m_f->glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
		
		stbi_image_free(image);
		fclose(f);
	}

	m_f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	m_f->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture;
}
//////////////////////////////////////////////////////////////////////////////
GLuint TextureManager::GetTexture(std::string textureName)
{
	auto it = std::find_if(m_textures.begin(), m_textures.end(), [textureName](auto entry)->bool { return entry.first == textureName; });
	if (it != m_textures.end())
	{
		return (*it).second;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void TextureManager::ReleaseTexture(std::string textureName)
{
	// TO DO
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
TextureManager* TextureManager::m_pInstance = nullptr;
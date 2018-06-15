#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLContext>
#include <unordered_map>
#include <string>

class TextureManager
{
public:
	void Init();
	GLuint LoadTextureImage(std::string, int format);
	GLuint LoadTexture(const char* textureFilePath, int format);
	GLuint LoadCubemapTexture(std::vector<std::string> faces, int format);
	GLuint GetTexture(std::string textureName);
	void ReleaseTexture(std::string textureName);

	static TextureManager* GetInstance();

	void SetGLContext(QOpenGLContext* context)
	{
		m_pGlContext = context;
		if (context)
		{
			m_f = context->versionFunctions<QOpenGLFunctions_4_5_Core>();
		}
	}

protected:
	TextureManager();
	~TextureManager();

private:
	std::unordered_map<std::string, GLuint> m_textures;
	QOpenGLContext *m_pGlContext = nullptr;
	QOpenGLFunctions_4_5_Core* m_f = nullptr;
	static TextureManager* m_pInstance;
};


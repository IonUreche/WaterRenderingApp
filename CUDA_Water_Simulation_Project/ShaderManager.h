#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLContext>

#include <vector>
#include <unordered_map>

enum
{
	SHADER_FLAG_VERTEX = 0x00000001,
	SHADER_FLAG_FRAGMENT = 0x000000002,
	SHADER_FLAG_TESS_CONTROL = 0x000000004,
	SHADER_FLAG_TESS_EVAL = 0x00000008,
	SHADER_FLAG_VERT_FRAG = SHADER_FLAG_VERTEX | SHADER_FLAG_FRAGMENT,
	SHADER_FLAG_ALL = SHADER_FLAG_VERTEX | SHADER_FLAG_FRAGMENT | SHADER_FLAG_TESS_CONTROL | SHADER_FLAG_TESS_EVAL
};

class ShaderManager
{
public:
	static ShaderManager* GetInstance();

	void InitShaders();

	void LoadShader(std::string path, uint32_t flags);
	GLuint GetShaderProgram(std::string path);

	void read_shader_src(const char *fname, std::vector<char> &buffer);
	GLuint load_and_compile_shader(const char *fname, GLenum shaderType);
	GLuint create_program(const char *path_vert_shader, const char *path_frag_shader,
		const char *path_tesc_shader, const char *path_tese_shader);

	void SetGLContext(QOpenGLContext* context)
	{
		m_pGlContext = context;
		if(context)
		{
			m_f = context->versionFunctions<QOpenGLFunctions_4_5_Core>();
		}
	}

protected:
	ShaderManager();
	~ShaderManager();

private:
	static ShaderManager* m_pInstance;
	QOpenGLContext *m_pGlContext = nullptr;
	QOpenGLFunctions_4_5_Core* m_f = nullptr;
	std::unordered_map<std::string, GLuint> m_shadersMap;
};


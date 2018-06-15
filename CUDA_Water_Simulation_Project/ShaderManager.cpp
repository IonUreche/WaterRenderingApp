#include "ShaderManager.h"

#include <fstream>
#include <cassert>
#include <iostream>

ShaderManager::ShaderManager()
{
}
/////////////////////////////////////////////////////////////////////////////////////
ShaderManager::~ShaderManager()
{
	if (m_pInstance)
		delete m_pInstance;
}
/////////////////////////////////////////////////////////////////////////////////////
ShaderManager* ShaderManager::GetInstance()
{
	if(m_pInstance == nullptr)
		m_pInstance = new ShaderManager();
	return m_pInstance;
}
/////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::InitShaders()
{
	//m_axisShader = create_program("src/Shaders/basic3D.vert", "src/Shaders/basic3D.frag", "", "");
	//m_lightCubeShader = create_program("src/Shaders/lightCubeShader.vert", "src/Shaders/lightCubeShader.frag", "", "");
	//m_terrainShader = create_program("src/Shaders/terrain.vert", "src/Shaders/terrain.frag",
	//	"src/Shaders/terrain.tesc", "src/Shaders/terrain.tese");
	//m_waterflowShader = create_program("src/Shaders/WaterFlow/waterflow.vert", "src/Shaders/WaterFlow/waterflow.frag",
	//	"src/Shaders/WaterFlow/waterflow.tesc", "src/Shaders/WaterFlow/waterflow.tese");
	//m_skyboxShader = create_program("src/Shaders/skybox.vert", "src/Shaders/skybox.frag", "", "");
	//m_filterShader = create_program("src/Shaders/customEffect.vert", "src/Shaders/customEffect.frag", "", "");
	LoadShader("src/Shaders/basic3D", SHADER_FLAG_VERT_FRAG);
	//LoadShader("src/Shaders/WaterFlow/waterflow", SHADER_FLAG_ALL);
	LoadShader("src/Shaders/skybox", SHADER_FLAG_VERT_FRAG);
	LoadShader("src/Shaders/terrain", SHADER_FLAG_ALL);
	LoadShader("src/Shaders/mesh", SHADER_FLAG_ALL);
	LoadShader("src/Shaders/water", SHADER_FLAG_ALL);
	LoadShader("src/Shaders/customEffect", SHADER_FLAG_VERT_FRAG);
	LoadShader("src/Shaders/textureDisplay1D", SHADER_FLAG_VERT_FRAG);
	LoadShader("src/Shaders/textureDisplay2D", SHADER_FLAG_VERT_FRAG);
}
GLuint ShaderManager::GetShaderProgram(std::string path)
{
	auto it = m_shadersMap.find(path);
	if(it == m_shadersMap.end())
		return 0;
	return (*it).second;
}
/////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::LoadShader(std::string path, uint32_t flags)
{
	size_t lastPos = -1;
	lastPos = path.find_last_of("/");
	std::string pathWithoutName = path.substr(0, lastPos);
	std::string ShaderName = path.substr(lastPos + 1);

	std::string vertShaderStr = (flags & SHADER_FLAG_VERTEX) ? pathWithoutName + "/" + ShaderName + ".vert" : "";
	std::string fragShaderStr = (flags & SHADER_FLAG_FRAGMENT) ? pathWithoutName + "/" + ShaderName + ".frag" : "";
	std::string tessControlShaderStr = (flags & SHADER_FLAG_TESS_CONTROL) ? pathWithoutName + "/" + ShaderName + ".tesc" : "";
	std::string tessEvalShaderStr = (flags & SHADER_FLAG_TESS_EVAL) ? pathWithoutName + "/" + ShaderName + ".tese" : "";

	GLuint shader = create_program(vertShaderStr.c_str(), fragShaderStr.c_str(), tessControlShaderStr.c_str(), tessEvalShaderStr.c_str());

	m_shadersMap[ShaderName] = shader;
}
/////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::read_shader_src(const char *fname, std::vector<char> &buffer)
{
	std::ifstream f(fname);
	std::string str;
	if (f)
	{
		f.seekg(0, std::ios::end);
		str.reserve(f.tellg());
		f.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());

		// IMPROVE: AVOID copy from string to vector<char>
		buffer.assign(str.begin(), str.end());
	}
	else
	{
		assert(false);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
GLuint ShaderManager::load_and_compile_shader(const char *fname, GLenum shaderType) {
	// Load a shader from an external file
	std::vector<char> buffer;
	read_shader_src(fname, buffer);
	buffer.push_back('\0');
	const char *src = &buffer[0];

	// Compile the shader
	GLuint shader = m_f->glCreateShader(shaderType);
	m_f->glShaderSource(shader, 1, &src, NULL);
	m_f->glCompileShader(shader);
	// Check the result of the compilation
	GLint test;
	m_f->glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if (!test) {
		std::cerr << "Shader compilation failed with this message:" << std::endl;
		std::vector<char> compilation_log(512);
		m_f->glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
		std::cerr << &compilation_log[0] << std::endl;
		char c;
		std::cin >> c;
		exit(-1);
		return -1;
	}
	return shader;
}
/////////////////////////////////////////////////////////////////////////////////////
GLuint ShaderManager::create_program(const char *path_vert_shader, const char *path_frag_shader,
	const char *path_tesc_shader, const char *path_tese_shader) {

	GLuint shaderProgram = m_f->glCreateProgram();

	// Load and compile the vertex and fragment shaders
	if (strcmp(path_vert_shader, "") != 0)
	{
		GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
		m_f->glAttachShader(shaderProgram, vertexShader);
		//glDeleteShader(vertexShader);
	}
	else
	{
		assert(false);
	}

	if (strcmp(path_frag_shader, "") != 0)
	{
		GLuint fragmentShader = load_and_compile_shader(path_frag_shader, GL_FRAGMENT_SHADER);
		m_f->glAttachShader(shaderProgram, fragmentShader);
		m_f->glDeleteShader(fragmentShader);
	}
	else
	{
		assert(false);
	}

	if (strcmp(path_tesc_shader, "") != 0)
	{
		GLuint tessControlShader = load_and_compile_shader(path_tesc_shader, GL_TESS_CONTROL_SHADER);
		m_f->glAttachShader(shaderProgram, tessControlShader);
		m_f->glDeleteShader(tessControlShader);
	}

	if (strcmp(path_tese_shader, "") != 0)
	{
		GLuint tessEvalShader = load_and_compile_shader(path_tese_shader, GL_TESS_EVALUATION_SHADER);
		m_f->glAttachShader(shaderProgram, tessEvalShader);
		m_f->glDeleteShader(tessEvalShader);
	}

	// Link and use the program
	m_f->glLinkProgram(shaderProgram);
	GLenum glGetError1 = m_f->glGetError();
	
	m_f->glUseProgram(shaderProgram);
	glGetError1 = m_f->glGetError();

	return shaderProgram;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
ShaderManager* ShaderManager::m_pInstance = nullptr;

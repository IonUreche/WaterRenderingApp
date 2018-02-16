#include "GerstnelParamManager.h"
#include <fstream>
#include <cassert>
const char* k_filename = "gerstnel_params.txt";

GerstnelParamManager::GerstnelParamManager()
{
	LoadFromFile();
}

GerstnelParamManager::~GerstnelParamManager()
{
}

void GerstnelParamManager::LoadFromFile()
{
	std::ifstream fin(k_filename);
	fin >> m_count;

	m_params.resize(24 * m_count);
	m_names.resize(m_count);

	for (int i = 0; i < m_count; ++i)
	{
		fin >> m_names[i];
		for(int j = 0; j < 24; ++j)
			fin >> m_params[24 * i + j];
	}

	fin.close();
}

void GerstnelParamManager::SaveToFile()
{
	std::ofstream fout(k_filename);
	fout << m_count << "\n";

	for (int i = 0; i < m_count; ++i)
	{
		fout << m_names[i] << "\n";
		for (int j = 0; j < 24; ++j)
		{
			fout << m_params[24 *i + j];
			if((j+1) % 6 == 0) fout << '\n'; else fout << ' ';
		}
	}

	fout.close();
}
// true if new config created
// false if old one is updated
bool GerstnelParamManager::AddConfig(const char *config_name, float params[24])
{
	int index = -1;
	for (int i = 0; i < m_names.size(); ++i)
	{
		if (m_names[i] == config_name)
		{
			index = i;
		}
	}
	if (index != -1)
	{
		for (int i = 0; i < 24; ++i)
		{
			m_params[index * 24 + i] = params[i];
		}
		return false;
	}
	else
	{
		++m_count;
		m_names.push_back(config_name);
		for (int i = 0; i < 24; ++i)
		{
			m_params.push_back(params[i]);
		}
		return true;
	}
}

std::vector<float> GerstnelParamManager::GetConfigData(int index)
{
	assert(m_params.size() >= 24 * index);
	std::vector<float> ret(m_params.begin() + index * 24, m_params.begin() + (index + 1) * 24);
	return ret;
}
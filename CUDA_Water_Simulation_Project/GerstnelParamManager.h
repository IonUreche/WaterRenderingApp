#pragma once
#include <vector>
#include <string>

class GerstnelParamManager
{
public:
	GerstnelParamManager();
	~GerstnelParamManager();

	bool AddConfig(const char *config_name, float params[24]);
	void SaveToFile();

	std::vector<std::string>& GetConfigsNames() { return m_names; }
	std::vector<float> GetConfigData(int index);

protected:
	void LoadFromFile();
private:
	int m_count = 0;
	std::vector<float> m_params;
	std::vector<std::string> m_names;
};


#include "../../src/base_framework/log4J/log.h"
#include "../../src/base_framework/config/config.h"
#include "yaml-cpp/yaml.h"
#include <time.h>
configing::ConfigVar<int>::ptr g_int_value_config = configing::Config::LookUp("system.port", (int)8080, "system.port");

// Yaml解析
void PrintYaml(const YAML::Node& node, int level)
{
    if (node.IsScalar()) {
        LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if (node.IsNull()) {
        LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ') 
                << it->first << " - " << it->second.Type() << " - " << level;
            PrintYaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            PrintYaml(node[i], level + 1);
        }
    }
}

// Yaml测试
void TestYaml()
{
    YAML::Node root = YAML::LoadFile("/home/Nihaowa123/server_framework/bin/config/log.yaml");
    PrintYaml(root, 0);
    // LOG_INFO(LOG_ROOT()) << root;
}

// 测试LoadFromYaml
void TestLoadFromYaml()
{
    LOG_INFO(LOG_ROOT()) << g_int_value_config->GetName();
    LOG_INFO(LOG_ROOT()) << g_int_value_config->ToString();
  
    YAML::Node root = YAML::LoadFile("/home/Nihaowa123/server_framework/bin/config/log.yaml");
    configing::Config::LoadFromYaml(root);
    LOG_INFO(LOG_ROOT()) << g_int_value_config->GetName();
    LOG_INFO(LOG_ROOT()) << g_int_value_config->ToString(); 
}
int main(int argc, char **argv)
{
    // LOG_INFO(LOG_ROOT()) << g_int_value_config->GetName();
    // LOG_INFO(LOG_ROOT()) << g_int_value_config->ToString();
    // LOG_INFO(LOG_ROOT()) << g_int_value_config->GetDescription();

    TestLoadFromYaml();

    return 0;
}
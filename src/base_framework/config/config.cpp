#include "config.h"

namespace configing {

Config::ConfigVarMap Config::s_configvarmap;

ConfigVarBase::ptr Config::LookUpBase(const std::string& name)
{
    auto it = s_configvarmap.find(name);
    if (it != s_configvarmap.end()) {
        return it->second;
    }
    return nullptr;
}
// "A.B", 10
// 转换成
// A:
//   B: 10
static void ListAllMember(const std::string& prefix, const YAML::Node& node, 
                          std::list<std::pair<std::string, const YAML::Node>>& output)
{
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
            LOG_ERROR(LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
            return;
    }
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for(auto it = node.begin(); it != node.end(); ++it) {
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), 
                          it->second, output);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node& root)
{
    std::list<std::pair<std::string, const YAML::Node>> allNodes;
    ListAllMember("", root, allNodes);
    for (auto& i : allNodes) {
        std::string key = i.first;
        if (key.empty()) {
            continue;
        }
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookUpBase(key);
        if (var) {
            if (i.second.IsScalar()) {
                var->FromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->FromString(ss.str());
            }
        }
    }
}

}
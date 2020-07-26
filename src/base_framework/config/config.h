#ifndef __CONFIG__H__
#define __CONFOG__H__

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "../../../src/base_framework/log4J/log.h"

namespace configing {

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
                 : m_configname(name)
                 , m_configdescription(description) {
                 }

    virtual ~ConfigVarBase() {}
    const std::string& GetName() const { return m_configname; }
    const std::string& GetDescription() const { return m_configdescription; }
    // 转成字符串,反序列化
    virtual std::string ToString() = 0;
    // 序列化
    virtual bool FromString(const std::string& val) = 0;
protected:
    std::string m_configname;
    std::string m_configdescription;
private:

};

template<class T>
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;

    ConfigVar(const std::string& name, const T& defaultValue, const std::string& description = "") 
            : ConfigVarBase(name, description), m_configval(defaultValue)
    { }

    // 转成字符串,反序列化 
    std::string ToString() override
    {
        try {
            return boost::lexical_cast<std::string>(m_configval);
        } catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::ToString exception" << e.what() << "convert: " 
                                  << typeid(m_configval).name() << "to string";
        }
        return "";
    }

    // 将YAML String 转成参数的值
    bool FromString(const std::string& configval) override
    {
        try {
            m_configval = boost::lexical_cast<T>(configval);
        } catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::FromString exception" << e.what() << "convert: string to" 
                                  << typeid(m_configval).name();
        }
        return false;
    }
private:
    T m_configval;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    // 返回对应的配置参数,如果参数名存在但是类型不匹配则返回nullptr
    template<class T>
    static typename ConfigVar<T>::ptr LookUp(const std::string& name, const T& defaultValue, const std::string& description = "")
    {
        auto it = s_configvarmap.find(name);
        if (it != s_configvarmap.end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if(tmp) {
                LOG_INFO(LOG_ROOT()) << "LookUp name = " << name << "exists";
                return tmp;
            } else {
                return nullptr;
            }
        }
        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
            LOG_ERROR(LOG_ROOT()) << "LookUp name invalid" << name;
            throw std::invalid_argument(name);
        }
        typename ConfigVar<T>::ptr configVarptr(new ConfigVar<T>(name, defaultValue, description));
        s_configvarmap[name] = configVarptr;
        return configVarptr;
    }

    // 查找配置参数
    template<class T>
    static typename ConfigVar<T>::ptr LookUp(const std::string& name)
    {
        auto it = s_configvarmap.find(name);
        if (it == s_configvarmap.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

private:
    static ConfigVarMap s_configvarmap;
};

} 

#endif
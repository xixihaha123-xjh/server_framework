#include "../../src/base_framework/log4J/log.h"
#include "../../src/base_framework/config/config.h"

configing::ConfigVar<int>::ptr g_int_value_config = configing::Config::LookUp("system.port", (int)8080, "system,port");

int main (int argc, char **argv)
{
    LOG_INFO(LOG_ROOT()) << g_int_value_config->GetName();
    LOG_INFO(LOG_ROOT()) << g_int_value_config->ToString();
    LOG_INFO(LOG_ROOT()) << g_int_value_config->GetDescription();
}
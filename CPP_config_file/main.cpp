//main.cpp
#include "Config.h"

#if 0
int main()
{
    int port;
    std::string ipAddress;
    std::string username;
    std::string password;
    const char ConfigFile[]= "config.txt";
    Config configSettings(ConfigFile);

    //port = configSettings.Read("port", 0);
    port = configSettings.Read<int>("port");  // 0 indicate int. (could use 1,2,... and so on)
    //ipAddress = configSettings.Read("ipAddress", ipAddress);
    ipAddress = configSettings.Read<std::string>("ipAddress");
    username = configSettings.Read("username", username);
    password = configSettings.Read("password", password);
    
    std::cout<<"port:"<<port<<std::endl;
    std::cout<<"ipAddress:"<<ipAddress<<std::endl;
    std::cout<<"username:"<<username<<std::endl;
    std::cout<<"password:"<<password<<std::endl;


    return 0;
}
#else
int main()
{
    bool autorun_state;
    bool autorun_cycle;
    std::string AT_V2XFREQ;
    std::string destination_l2_id1;
    
    const char ConfigFile[]= "pc5-setup.ini";
    Config configSettings(ConfigFile);

    autorun_state = configSettings.Read<bool>("autorun_state");  // 0 indicate int. (could use 1,2,... and so on)
    autorun_cycle = configSettings.Read<bool>("autorun_cycle");
    AT_V2XFREQ = configSettings.Read<std::string>("AT^V2XFREQ");

    int i=1;
    configSettings.Read("destination_l2_id" + std::to_string(i), destination_l2_id1);  // RIGHT!
    
    std::cout<<"autorun_state:"<<autorun_state<<std::endl;
    std::cout<<"autorun_cycle:"<<autorun_cycle<<std::endl;
    std::cout<<"AT_V2XFREQ:"<<AT_V2XFREQ<<std::endl;
    std::cout<<"destination_l2_id1:"<<destination_l2_id1<<std::endl;


    return 0;
}


#endif


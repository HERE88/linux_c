#include "Log.h"
 
int main()
{
    //int max = 100000;
    int max = 1;
    
    InitLogger(true);

    int a = 1000;

    while(max--)
    {
        LOG_TRACE("this is LOG_TRACE. a="<<a<<",hex="<<std::hex<<a);
        LOG_DEBUG("this is LOG_DEBUG.");
        LOG_INFO("this is LOG_INFO.");
        LOG_WARN("this is LOG_WARN.");
        LOG_ERROR("this is LOG_ERROR.");
        LOG_FATAL("this is LOG_FATAL.");
    }
    
    return 0;
}

#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/configurator.h>
 
#include "Log.h"

Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("logmain"));
 
void InitLogger(bool daemonized)
{
	if (daemonized)
		//PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("/your/path/log4cplusd.conf"));
		PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplusd.conf"));
	else
		//PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("/your/path/log4cplus.conf"));
		PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.conf"));
}

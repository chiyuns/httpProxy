#include "logger.h"
log4cplus::Logger CLogger::m_debug;
log4cplus::Logger CLogger::m_info;
log4cplus::Logger CLogger::m_error;
log4cplus::Logger CLogger::m_fatal;


bool CLogger::Init(const string &configPath,string &errmsg){
	if(configPath.empty()){
		errmsg ="arg:configer file is empty!";
		return false;
	}
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(configPath.c_str()));
	m_debug = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("DEBUG_MSGS"));
	m_info  = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("INFO_MSGS"));
	m_error = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ERROR_MSGS"));
	m_fatal = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("FATAL_MSGS"));
	return true;
}

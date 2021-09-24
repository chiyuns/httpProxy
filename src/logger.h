#pragma once

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>

#include <string>
using namespace std;
using namespace log4cplus;

#define MAX_LOG_MSG_LEN 4096
class CLogger{
public:
	static bool	Init(const string &configPath,string &errmsg);
public:
	static log4cplus::Logger m_debug;
	static log4cplus::Logger m_info;
	static log4cplus::Logger m_error;
	static log4cplus::Logger m_fatal;
};

//调试
#define LOGDBG(fmt,...){					\
	char logmsg[MAX_LOG_MSG_LEN] = {0};		\
	snprintf(logmsg,sizeof(logmsg),"|%s|%s|%ld|:" fmt,__FILE__,__func__,__LINE__,##__VA_ARGS__);		\
	LOG4CPLUS_DEBUG(CLogger::m_debug, logmsg);	\
}

//提示
#define LOGINFO(fmt,...){					\
	char logmsg[MAX_LOG_MSG_LEN] = {0};		\
	snprintf(logmsg,sizeof(logmsg),"|%s|%s|%ld|:" fmt,__FILE__,__func__,__LINE__,##__VA_ARGS__);		\
	LOG4CPLUS_INFO(CLogger::m_info, logmsg);	\
}

//错误
#define LOGERR(fmt,...){					\
	char logmsg[MAX_LOG_MSG_LEN] = {0};		\
	snprintf(logmsg,sizeof(logmsg),"|%s|%s|%ld|:" fmt,__FILE__,__func__,__LINE__,##__VA_ARGS__);		\
	LOG4CPLUS_ERROR(CLogger::m_error, logmsg);	\
}

//崩溃
#define LOGFATAL(fmt,...){					\
	char logmsg[MAX_LOG_MSG_LEN] = {0};		\
	snprintf(logmsg,sizeof(logmsg),"|%s|%s|%ld|:" fmt,__FILE__,__func__,__LINE__,##__VA_ARGS__);		\
	LOG4CPLUS_FATAL(CLogger::m_fatal, logmsg);	\
}

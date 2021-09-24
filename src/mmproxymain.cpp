/***********************************************************************
 * Copyright(c) Tencent 2010, all rights reserved
 *
 * File:        mmproxy.cpp
 * Author:      iotazhang
 * Revision:    $Id: mmproxy.cpp 281550 2012-03-21 15:25:45Z stephenliu $
 * Description:
 *
 ***********************************************************************/
#include <exception>
#include "logger.h"
#include "httpProxySvr.h"
#include <signal.h>
#include <assert.h>
#include "iServerConfig.h"
#include "zRedisConnectionPool.h"

using namespace std;
using namespace Comm;
using std::exception;
#define  LOG_FILE_PATH  "/home/qspace/conf/log4cplus.conf"

void initDaemon() {
    pid_t pid;
    if ((pid = fork()) < 0) { //创建进程失败
        exit(2);
    } else if (pid != 0) { //父进程退出
        exit(0);
    }

    //子进程
    assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);
    assert(signal(SIGALRM, SIG_IGN) != SIG_ERR);
    //assert(signal(SIGCHLD, SIG_IGN) != SIG_ERR);

    close(0);
    close(1);
    close(2);

    open("/dev/null", O_RDONLY);
    dup2(0, 1);
    dup2(0, 2);

    setsid();
}


int main(int argc, char** argv) {
	
    if( argc <= 2 ) {
        printf("Usage: %s -i [filename]...\n", argv[0]);
        return -1;
    }
	
	string errmsg;
	
	if(!CLogger::Init(LOG_FILE_PATH, errmsg))
	{
		cout<<"CLogger init failed,errmsg="<<errmsg<<endl;
		exit(-1);
	}
	
//	const char * sConfFile = "/home/qspace/eclipse-workspace/mmonlineproxy/src/onlineproxy_svr.conf";
	const char *conf = argv[2];
	CServerConfig  objConfig;
	if ( 0 != objConfig.ReadConfig(conf) )
	{
		cout<<"read file fail"<<endl;
		exit(-1);
	}

    string strDaemonize =objConfig.GetDaemonize();
	if(strDaemonize.compare("yes") == 0)
	{
		initDaemon();
	}


	RedisCfgInfo redisCfgInfo = objConfig.GetRedisCfgInfo();
	struct timeval timeout;
	timeout.tv_sec = redisCfgInfo.iRedisTimeOut;
	timeout.tv_usec =0; 
	ZRedisConnectionPool::init(redisCfgInfo.strRedisAddr,redisCfgInfo.strRedisPwd,timeout, redisCfgInfo.iRedisConNum);
	
    try 
	{
		  HttpProxyServer server;
		  server.Listen(objConfig.GetServerIP(), objConfig.GetServerPort());
		  return 0;
    } catch (const exception& ex) 
	{
    	LOGERR("ERR: %s, mmproxy exit", ex.what());
    } catch (...) {
    	LOGERR("ERR: mmproxy exit");
    }

    return 2;
}

//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL: http://gz-svn.tencent.com/gzrd/gzrd_mail_rep/MicroMsg_proj/trunk/MicroMsg/micromsg/mmproxy/proxy/mmproxy.cpp $ $Id: mmproxy.cpp 281550 2012-03-21 15:25:45Z stephenliu $ " GZRD_SVN_ATTR;
// gzrd_Lib_CPP_Version_ID--end


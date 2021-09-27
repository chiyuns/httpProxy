#include "httpProxySvr.h"
#include "logger.h"
#include "iConfig.h"
#include "zRedisConnectionPool.h"

using namespace Comm;

int HttpProxyServer::Listen(const char *host, int port) {
  init();
  m_svr.listen(host, port);
  return 0;
}

void HttpProxyServer::init() {

  /////////////////////////////////////////////////
  m_svr.Get("/GetUserOnlineStatus", [&](const Request &req, Response &res) {
  	string suid = req.get_param_value("userId");
	int uid = atoi(suid.c_str());
	
   	char result[256];
	MMOnlineRedisResult_t   redisResult;
	
	ZRedisConnection* pConn = Singleton<ZRedisConnectionPool>::Instance().Get();
	bool  bRet = pConn->query(uid, redisResult);
	Singleton<ZRedisConnectionPool>::Instance().Back(pConn);
	if(!bRet)
	{
	   snprintf(result, sizeof(result),
               "{\"ret\":%d,\"msg\":\"get user %d seq failed\"}", 201, uid);
       res.set_content(result, "text/json");
	   return;
	}
	
	int iClientIp = atoi(redisResult.strClientIp.c_str());
	
	struct in_addr addr1;
    memcpy(&addr1, &iClientIp, 4);    	
	char ipAddr[30];
	snprintf(ipAddr,30,"%s:%s",inet_ntoa(addr1),redisResult.strClientPort.c_str());
	
	string strIpAddr(ipAddr);
	
	string strUrl ="http://";
	strUrl += strIpAddr;

    snprintf(result, sizeof(result),
             "{\"ret\":200,\"msg\":\"get user %d online status success\",\"status\":%s,\"onlineTime\":%s,\"url\":\"%s\",\"clientId\":%s,\"userId\":%d}",
             uid, redisResult.strFlag.c_str(), redisResult.strOnlineTime.c_str(), strUrl.c_str(), redisResult.strClientId.c_str(), uid);
    res.set_content(result, "text/json");
  });

  m_svr.set_error_handler([](const Request & /*req*/, Response &res) {
    const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, res.status);
    res.set_content(buf, "text/html");
  });

  m_svr.set_logger([](const Request &req, const Response &res) {
    printf("%s\n", req.get_param_value("uid").c_str());
  });
}

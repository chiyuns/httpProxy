/***********************************************************************
 *
 * File:        OnlineManager.h
 * Author:      iotazhang
 * Revision:    主要是用于redis访问连接池
 * Description: 在线状态管理器, 记录一个mmproxy上所有客户端的在线状态
 *
 ***********************************************************************/

#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "iServerConfig.h"
#include "hiredis_cluster/hircluster.h"

using namespace std;

typedef struct tagMMOnlineRedisResult {
	std::string  strFlag;
	std::string  strClientId;
	std::string  strClientIp;
	std::string  strDeviceId;
	std::string  strClientPort;
	std::string  strOnlineTime;
} MMOnlineRedisResult_t;


class MMOnlineSvrConfig;

class ZRedisConnectionPool;


/**
 * 在线状态管理器
 */
class ZRedisConnection {
public:
	ZRedisConnection(const  RedisCfgInfo& redisCfgInfo);

    ~ZRedisConnection();
	
   
public:

	bool Connect();
	
    int query(unsigned int uin, MMOnlineRedisResult_t& result);

    int update(unsigned int uin, const MMOnlineRedisResult_t& result);
	
	bool hmset(const string& key, const std::map<std::string,string>& value);
	
	bool hmget(const string& key, const vector<string>&fileds, vector<string>& contents);

	bool onlineHset(const string& key, const string& filed, const string& val);
	
	bool onlineHget(const string& key, const string& filed, string& val);
	
	bool onlineExists(const string& key);
	
	bool onlineDel(const string& key);

	bool CheckReply(const redisReply* reply);

	void FreeReply(const redisReply* reply);

	bool expire(const std::string& key,  uint32_t second);
	
	
private:
   redisClusterContext *m_pContext;
   RedisCfgInfo  m_redisCfgInfo;
   
};

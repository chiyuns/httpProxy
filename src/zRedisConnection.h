/***********************************************************************
 *
 * File:        OnlineManager.h
 * Author:      iotazhang
 * Revision:    $Id: OnlineManager.h 328249 2012-07-24 07:26:44Z dirkcai $
 * Description: 在线状态管理器, 记录一个mmproxy上所有客户端的在线状态
 *
 ***********************************************************************/

#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Singleton.h"
#include "mmonlinesvrconfig.h"
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
	

	bool Connect();
	
    bool query(unsigned int uin, MMOnlineRedisResult_t& result);

    /**
     * 更新状态信息
     * @param list 状态信息列表
     */
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

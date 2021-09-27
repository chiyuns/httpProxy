
#pragma once
#include <vector>
#include "logger.h"
#include "iConfig.h" // for CConfig
#include "Singleton.h"


struct RedisCfgInfo
{
	string       strRedisAddr;
    string        strRedisPwd;
	unsigned     iRedisTimeOut;
	unsigned     iRedisConNum;

};


namespace Comm
{
	
typedef struct tagCacheConfigImpl
{
	char m_serverIp[ 32 ];
	int  m_serverport;
} CacheConfigImpl_t;


class CServerConfig
{
public:
    CServerConfig() = default;
    ~CServerConfig() = default;
    CServerConfig(const CServerConfig& rhs) = delete;
    CServerConfig& operator =(const CServerConfig& rhs) = delete;
	
	int ReadConfig( const char *configFile);

	const char * GetServerIP()
	{
		return m_ptImpl.m_serverIp;
	}
	
	int  GetServerPort()
	{
		return m_ptImpl.m_serverport;
	}


	/**
	 *   redis配置信息
	 */
	const RedisCfgInfo& GetRedisCfgInfo() const
	{

		return m_redisCfgInfo;
	}


	const string& GetDaemonize() const
	{

		return m_daemonize;
	}

private:
	CacheConfigImpl_t  m_ptImpl;
	RedisCfgInfo     m_redisCfgInfo;
	string           m_daemonize;
};

}


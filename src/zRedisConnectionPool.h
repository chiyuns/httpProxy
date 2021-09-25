/***********************************************************************
 *
 * File:        OnlineManager.h
 * Author:      iotazhang
 * Revision:    $Id: OnlineManager.h 328249 2012-07-24 07:26:44Z dirkcai $
 * Description: 在线状态管理器, 记录一个mmproxy上所有客户端的在线状态
 *
 ***********************************************************************/

#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <list>

#include "zRedisConnection.h"
using namespace std;

#define MIN_CACHE_CONN_CNT	2


class ZRedisConnectionPool {

public:
	bool init(const  RedisCfgInfo& redisCfgInfo);
	
	bool unInit();
	
	ZRedisConnection* Get();

	void Back(ZRedisConnection* pCacheConn);

private:
    std::mutex mtx_;     

	int			m_curConnCnt;
	int 		m_maxConnCnt;
    std::condition_variable    m_condition;
    list<ZRedisConnection*> 	connect_pool_;

	RedisCfgInfo  m_redisCfgInfo;


};



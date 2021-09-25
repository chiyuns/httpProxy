/***********************************************************************
 *
 * File:        zRedisConnectionPool.cpp
 * Author:      lixin
 * Revision:    $Id: OnlineManager.cpp 388199 2013-01-05 14:49:12Z iotazhang $
 * Description: redis连接池
 *
 ***********************************************************************/
#include <unistd.h>
#include <thread>
#include <functional>
#include "zRedisConnection.h"
#include "zRedisConnectionPool.h"
#include "logger.h"


using namespace std;

bool ZRedisConnectionPool::init(const  RedisCfgInfo& redisCfgInfo) 
{
    m_redisCfgInfo = redisCfgInfo;
	m_curConnCnt = MIN_CACHE_CONN_CNT;
	m_maxConnCnt = redisCfgInfo.iRedisConNum;

	std::unique_lock<std::mutex> lk(mtx_);
	for (unsigned i = 0; i < m_curConnCnt; ++i) {
		 ZRedisConnection* pConn = new ZRedisConnection(redisCfgInfo);
		 if(pConn->Connect())
		 {
			 connect_pool_.push_back(pConn);
		 }else{
			// LOG(ERROR) << "connect redirs :" <<ip << ":" << port << pwd << "failed";
			delete pConn;
			return false;
		 }
	 }

	return true;
}


bool ZRedisConnectionPool::unInit()
{
	std::unique_lock<std::mutex> lk(mtx_);

	for (list<ZRedisConnection*>::iterator it = connect_pool_.begin(); it != connect_pool_.end(); it++) {
		ZRedisConnection* pConn = *it;
		delete pConn;
	}

    connect_pool_.clear();
	m_curConnCnt = 0;

}

ZRedisConnection* ZRedisConnectionPool::Get() 
{
    std::unique_lock<std::mutex> lk(mtx_);
	while(connect_pool_.empty())
	{
		if(m_curConnCnt >= m_maxConnCnt)
		{
			m_condition.wait(lk);
		}
		else
		{
			ZRedisConnection* pConn = new ZRedisConnection(m_redisCfgInfo);
			if(pConn->Connect())
			{
				connect_pool_.push_back(pConn);
				m_curConnCnt++;
			}else
			{
				delete pConn;
				return nullptr;
			}
		}

	}
	
	ZRedisConnection*  pConn = connect_pool_.front();
	connect_pool_.pop_front();
	
    return pConn;
}

void ZRedisConnectionPool::Back(ZRedisConnection* pCacheConn) 
{
    std::unique_lock<std::mutex> lk(mtx_);

	list<ZRedisConnection*>::iterator it = connect_pool_.begin();
	for (; it != connect_pool_.end(); it++) 
	{
		if (*it == pCacheConn) {
			break;
		}
	}

	if (it == connect_pool_.end()) 
	{
		connect_pool_.push_back(pCacheConn);
	}

	m_condition.notify_all();
}



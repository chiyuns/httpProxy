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
//using Comm::RichLogErr;

std::vector<std::shared_ptr<ZRedisConnection>> ZRedisConnectionPool::connect_pool_;
std::mutex ZRedisConnectionPool::mtx_;
int ZRedisConnectionPool::maxIdleTime_ = 2;

bool ZRedisConnectionPool::init(const std::string& strRedisAddr, const std::string& strRedisPwd, struct timeval timeout, unsigned iPoolSize, unsigned maxIdleTime) 
{
	bool flag = true;
	 if(iPoolSize <= 0)
	 {
		 flag = false;
	 }
	 std::unique_lock<std::mutex> lk(mtx_);
	 for (unsigned i = 0; i < iPoolSize; ++i) {
		 //实例化连接
		 ZRedisConnection* con = new ZRedisConnection();
		 if(con->Connect(strRedisAddr,strRedisPwd,timeout))
		 {
			 //放入连接池
			 connect_pool_.push_back(std::shared_ptr<ZRedisConnection>(con));
		 }else{
			// LOG(ERROR) << "connect redirs :" <<ip << ":" << port << pwd << "failed";
		 }
	 }
	 unsigned real_iPoolSize = connect_pool_.size();
	 if(iPoolSize > real_iPoolSize)
	 {
		 //LOG(ERROR) << "redis pool init failed! hope pool size:" << iPoolSize << "real size is " << real_iPoolSize;
		 flag = false;
	 }else{
		 //LOG(INFO) << "redis pool init success! pool size:" << real_iPoolSize;
	 }

	 //创建保活线程池
	//std::thread scanner(std::bind(&ZRedisConnectionPool::KeepAlive));
	//scanner.detach();

	maxIdleTime_  =  maxIdleTime;  
	
	 return flag;
}


std::shared_ptr<ZRedisConnection> ZRedisConnectionPool::Get() 
{
    std::unique_lock<std::mutex> lk(mtx_);
    if(connect_pool_.size() == 0)
    {
        return  nullptr;
    }
    //从连接容器里返回一个连接
    std::shared_ptr<ZRedisConnection> tmp = connect_pool_.front();
    connect_pool_.erase(connect_pool_.begin());
    return tmp;
}

void ZRedisConnectionPool::Back(std::shared_ptr<ZRedisConnection> &con) {
    std::unique_lock<std::mutex> lk(mtx_);
    //归还到容器
    connect_pool_.push_back(con);
}

int ZRedisConnectionPool::size() 
{
    std::unique_lock<std::mutex> lk(mtx_);
    int size = connect_pool_.size();
    return size;
}

//此块代码中保活ping不通，可能跟集群有关系
void ZRedisConnectionPool::KeepAlive() 
{
  while (true) {
    //定时保活
    std::this_thread::sleep_for(std::chrono::seconds(maxIdleTime_)); //目前定为10秒保活一次
    std::shared_ptr<ZRedisConnection> conn = ZRedisConnectionPool::Get();
    if(conn == nullptr )
	{
		LOGERR("can't get connection");
		return;
	}
	if(!conn->Ping())
	{
		LOGERR("redis ping error");
		if(conn->ReConnect())
		{
			LOGERR("redis ReConnect success");
			
		}
	}
	Back(conn);
  }
}

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

#include "zRedisConnection.h"
using namespace std;


class ZRedisConnectionPool {

public:
    /**
     * 初始化连接池
     * @param ip    服务ip
     * @param port  服务端口
     * @param pwd   服务密码
     * @param db    库
     * @param iPoolSize 连接池大小 
     * @return 
     */
    static bool init(const std::string& strRedisAddr, const std::string& strRedisPwd, struct timeval timeout, unsigned iPoolSize = 5, unsigned  maxIdleTime = 2);

    /**
     * 获取连接池的一个连接
     * @return 
     */
    static std::shared_ptr<ZRedisConnection> Get();

    /**
     * 将获取的连接返回连接池
     * @param con 
     */
    static void Back(std::shared_ptr<ZRedisConnection> &con);

    
    /**
     * 获取当前连接池可用连接资源数
     * @return 
     */
    static int size();

private:
	static void KeepAlive();     //扫描队列，保活线程池

private:
    static std::mutex mtx_;         //资源锁，防止多线程操作连接池
    static std::vector<std::shared_ptr<ZRedisConnection>> connect_pool_;  //连接池容器
    static int maxIdleTime_;

};




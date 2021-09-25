/***********************************************************************
 *
 * File:        ZRedisConnection.cpp
 * Author:      iotazhang
 * Revision:    $Id: ZRedisConnection.cpp 388199 2013-01-05 14:49:12Z iotazhang $
 * Description: Âú®Á∫øÁä∂ÊÄÅÁÆ°ÁêÜÂô®
 *
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include<iostream>
#include <unistd.h>
#include "zRedisConnection.h"
#include "hiredis_cluster/hircluster.h"
#include "cipherEncryptor.h"
#include "logger.h"

using namespace std;

#define  REDIS_EXPIRE_TIME    3600

ZRedisConnection::ZRedisConnection(const  RedisCfgInfo& redisCfgInfo)
{
	m_redisCfgInfo = redisCfgInfo;
	m_pContext = NULL;
}


ZRedisConnection :: ~ZRedisConnection()
{
	if (m_pContext) 
	{
		redisClusterFree(m_pContext);
		m_pContext = NULL;
	}
}

bool ZRedisConnection::Connect()
{	

	struct timeval timeout;
	timeout.tv_sec = m_redisCfgInfo.iRedisTimeOut;
	timeout.tv_usec =0; 

	m_pContext = redisClusterContextInit();
    redisClusterSetOptionAddNodes(m_pContext, m_redisCfgInfo.strRedisAddr.c_str());
    string  strDecryPwd = DecryptPass(m_redisCfgInfo.strRedisPwd);
	redisClusterSetOptionPassword(m_pContext, strDecryPwd.c_str());
    redisClusterSetOptionConnectTimeout(m_pContext, timeout);
    redisClusterSetOptionRouteUseSlots(m_pContext);
    redisClusterConnect2(m_pContext);
    if (m_pContext && m_pContext->err) 
	{
	   LOGERR("Error: %s", m_pContext->errstr);
	   return false;
    }
	return true;
}



bool ZRedisConnection::hmset(const string& key,const std::map<std::string,string>& value)
{
    bool bRet = false;

	std::stringstream ss;
    ss << "HMSET " << key << " ";
	 
	for (auto &it : value) 
	{
		ss << it.first << " " << it.second << " ";
	}
	std::string cmd = ss.str();
	cout<<cmd<<endl;
	
	LOGINFO("HMSET: %s\n", cmd.c_str());
	
	redisReply *reply = (redisReply *)redisClusterCommand(m_pContext, cmd.c_str());
	if(CheckReply(reply))
	{
		bRet = true;
        if (REDIS_REPLY_STRING == reply->type) {
            if (!reply->len || !reply->str || strcasecmp(reply->str, "OK") != 0) {
                bRet = false;
            }
        }

	}
    FreeReply(reply);
    return bRet;


}


bool ZRedisConnection::hmget(const string& key, const vector<string>&fileds, vector<string>&contents)
{
	bool bRet = false;
	std::stringstream ss;
    ss << "HMGET " << key << " ";
	for (vector<string>::const_iterator iter = fileds.cbegin(); iter != fileds.cend(); iter++)
    {
		ss << (*iter) << " ";
    }
	std::string cmd = ss.str();
	LOGDBG("HMGET: %s\n", cmd.c_str());
	redisReply *reply = (redisReply *)redisClusterCommand(m_pContext, cmd.c_str());
	if(CheckReply(reply))
	{
		 for (unsigned i = 0; i < reply->elements; i++) 
		 {
			string strContent ="";
			strContent.assign(reply->element[i]->str, reply->element[i]->len);
			contents.push_back(strContent);
		 }
		 bRet = true;
	}
    FreeReply(reply);
	return bRet;
	
}



bool ZRedisConnection::onlineHset(const string& key, const string& filed, const string& val)
{
    bool bRet = false;
	redisReply *reply = (redisReply *)redisClusterCommand(m_pContext, "HSET %s %s %s", key.c_str(), filed.c_str(), val.c_str() );
	if(CheckReply(reply))
	{
		bRet = true;
        if (REDIS_REPLY_STRING == reply->type) {
            if (!reply->len || !reply->str || strcasecmp(reply->str, "OK") != 0) {
                bRet = false;
            }
        }

	}
    FreeReply(reply);
	return bRet;
}


bool ZRedisConnection::CheckReply(const redisReply* reply)
{
	if (NULL == reply){
		  return false;
	 }

	  switch (reply->type) {
	  case REDIS_REPLY_STRING: {
		  return true;
	  }
	  case REDIS_REPLY_ARRAY: {
		  return true;
	  }
	  case REDIS_REPLY_INTEGER: {
		  return true;
	  }
	  case REDIS_REPLY_NIL: {
		  return false;
	  }
	  case REDIS_REPLY_STATUS: {
		  return true;
	  }
	  case REDIS_REPLY_ERROR: {
		  return false;
	  }
	  default: {
		  return false;
	  }
	  }

	  return false;

}


void ZRedisConnection::FreeReply(const redisReply* reply)
{
    if (NULL != reply) {
        freeReplyObject((void*)reply);
    }
}


bool ZRedisConnection::onlineHget(const string& key, const string& filed, string& val)
{
    bool bRet = false;
	redisReply *reply = (redisReply *)redisClusterCommand(m_pContext, "HGET %s %s ", key.c_str(), filed.c_str());
	if(CheckReply(reply))
	{
		val.assign(reply->str, reply->len);
        bRet = true;
    }
    FreeReply(reply);
	return bRet;
}

bool ZRedisConnection::onlineExists(const string& key)
{	
    bool bRet = false;
    if (0 == key.length())
	{
        return false;
    }
	
    redisReply *reply = (redisReply *)redisClusterCommand(m_pContext, "EXISTS %s", key.c_str());
	if(CheckReply(reply))
	{
		bRet = true;
		if (REDIS_REPLY_STRING == reply->type) {
			if (!reply->len || !reply->str || strcasecmp(reply->str, "OK") != 0) {
				bRet = false;
			}
		}

	}
	FreeReply(reply);
	return true;
}


bool ZRedisConnection::onlineDel(const string& key)
{	  
	bool bRet = false;
    if (0 == key.length())
	{
        return false;
    }

    redisReply *reply = (redisReply *)redisClusterCommand(m_pContext, "DEL %s", key.c_str());
	 if (CheckReply(reply)) 
	 {
        if (REDIS_REPLY_STATUS == reply->type) 
		{
            bRet = true;
        }
		else 
		{
            bRet = (reply->integer == 1) ? true : false;
        }
    } 
    FreeReply(reply);
	return true;
}




bool ZRedisConnection::expire(const std::string& key,  uint32_t second)
{
	bool bRet  = false;
	redisReply *reply = (redisReply *)redisClusterCommand(m_pContext,  "EXPIRE %s %u", key.c_str(), second);
	if (CheckReply(reply)) 
	{
		if (REDIS_REPLY_INTEGER == reply->type) 
		{
			int  ret = reply->integer;
			if(ret == 1)
			{
				bRet = true;
			}
		}
    } 
	FreeReply(reply);
	return bRet;
}


//≤È—Ø
int ZRedisConnection::query(unsigned int uin, MMOnlineRedisResult_t& result)
{
	string strKey = "onRedisLine_"+ to_string(uin);	
    vector<string>fileds;
	fileds.push_back("Flag");
	fileds.push_back("iClientId");
	fileds.push_back("iClientIp");
	fileds.push_back("iOnlineTime");
	fileds.push_back("cDeviceId");
	fileds.push_back("iClientPort");
	vector<string>values;
	if(!hmget(strKey, fileds, values))
	{
		LOGERR("hmget strKey:%s failed", strKey.c_str());
		return -1;
	}
	
	result.strFlag       = values[0];
	result.strClientId   = values[1];
	result.strClientIp   = values[2];
	result.strOnlineTime = values[3];
	result.strDeviceId   = values[4];
	result.strClientPort = values[5];
	
	LOGINFO("query %s:%s,%s,%s,%s,%s,%s\n",strKey.c_str(), result.strFlag.c_str(), result.strClientId.c_str(), result.strClientIp.c_str(),result.strOnlineTime.c_str(),result.strDeviceId.c_str(), result.strClientPort.c_str());
	return 0;
}


//∏¸–¬
int ZRedisConnection::update(unsigned int uin, const MMOnlineRedisResult_t& result)
{
	string strKey = "onRedisLine_"+ to_string(uin);
	std::map<std::string,string> mapValue;
	mapValue.insert(pair<string, string>("Flag", result.strFlag));  
    mapValue.insert(pair<string, string>("iClientId",  result.strClientId));  
	mapValue.insert(pair<string, string>("iClientIp",  result.strClientIp));  
	mapValue.insert(pair<string, string>("iOnlineTime", result.strOnlineTime));  
	mapValue.insert(pair<string, string>("cDeviceId", result.strDeviceId));  
	mapValue.insert(pair<string, string>("iClientPort", result.strClientPort));  
	hmset(strKey, mapValue);
	return 0;
}

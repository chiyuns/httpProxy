
#include <stdlib.h>
#include <string.h>
#include "iServerConfig.h"
#include "iString.h"

namespace Comm
{
	int CServerConfig :: ReadConfig( const char *configFile)
	{
		int ret=0;
		CConfig config(configFile);
		ret = config.Init();
		if ( 0 == ret )
		{
			const char * sectionName = "General";
			ConfigItemInfo_t infoArray[] = {
				CONFIG_ITEM_STR( sectionName,  "serverIp",      m_ptImpl.m_serverIp ),
				CONFIG_ITEM_INT( sectionName,  "severport",     m_ptImpl.m_serverport ),
				CONFIG_ITEM_END
				};

			ret = ConfigRead(&config, infoArray );


			string basesection ="Base";
			config.ReadItem(basesection, "daemonize", "yes",  m_daemonize);
			
			
			string redissection ="redis";
			config.ReadItem(redissection, "redisAddr", "",  m_redisCfgInfo.strRedisAddr);
			config.ReadItem(redissection, "redisPwd", "",  m_redisCfgInfo.strRedisPwd);
			config.ReadItem(redissection, "redisTimeOut", 5,  m_redisCfgInfo.iRedisTimeOut);
			config.ReadItem(redissection, "redisConNum", 5,  m_redisCfgInfo.iRedisConNum);

			ConfigDump( infoArray );
		}

		return ret;
	}
}

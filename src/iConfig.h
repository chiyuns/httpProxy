//Config.hh define common config 
//read and write interface.
#pragma once

#include <string>
#include <map>
#include <vector>
#include "logger.h"

/**
 *\file Config.hh
 *\brief define common config 
 * read and write interface.
 */

namespace Comm {

#define CONFIG_TYPE_NON 0
#define CONFIG_TYPE_STR 1
#define CONFIG_TYPE_INT 2
#define CONFIG_TYPE_UIN 3
#define CONFIG_TYPE_HEX 4 

typedef struct tagConfigItemInfo {
	const char * sSection;
	const char * sKey;
	int  iType;
	void * pvAddr;
	int  iSize;
} ConfigItemInfo_t;

#ifndef CONFIG_ITEM_STR
#define CONFIG_ITEM_STR(section, key, var) \
	{ section, key, CONFIG_TYPE_STR, (void*)var, sizeof( var ) }
#endif

#ifndef CONFIG_ITEM_INT
#define CONFIG_ITEM_INT(section, key, var) \
	{ section, key, CONFIG_TYPE_INT, (void*)&(var), sizeof( var ) }
#endif

#ifndef CONFIG_ITEM_UIN
#define CONFIG_ITEM_UIN(section, key, var) \
	{ section, key, CONFIG_TYPE_UIN, &(var), sizeof( var ) }
#endif

#ifndef CONFIG_ITEM_HEX
#define CONFIG_ITEM_HEX(section, key, var) \
	{ section, key, CONFIG_TYPE_HEX, &(var), sizeof( var ) }
#endif

#ifndef CONFIG_ITEM_END
#define CONFIG_ITEM_END \
	{ "", "", CONFIG_TYPE_NON, NULL, 0 }
#endif

class CConfig;

/**
 * @return 0 : 所有配置项都成功读取
 * @return -1 : 有些配置项不存在，通过查看日志可以知道是哪个配置项
 */
extern int ConfigRead( CConfig * config, ConfigItemInfo_t * infoArray );

extern void ConfigDump( ConfigItemInfo_t * infoArray );

/**
 * CConfig is used to 
 * read and write ini file 
 * format config file.
 */
class CConfig
{
public:

	inline CConfig() {}

	CConfig & operator= (const CConfig& file);

	explicit  inline CConfig(const CConfig& file)
	{ operator=(file); }

	/**
	 * Constructor
	 */       
	CConfig(const std::string& configfile);

	/**
	 * Desctructor
	 */
	virtual  ~CConfig(void);

	const std::string & getConfigFile();

	/**
	 * Used to get all section title.
	 * @param sectionlist indicates section title.
	 * @return 0 on success.
	 *         -1 on failure.
	 */
	int getSectionList(
		std::vector<std::string>& sectionlist
	);

	/**
	 * Used to get a section .
	 * @param name indicates section name.
	 * @param section indicates section value.
	 * @return 0 on success.
	 *         -1 on failure.
	 */
	int getSection(
		const std::string& name,
		std::map<std::string,std::string>& section
	);

	/**
	 * Used to get a section .
	 * @param name indicates section name.
	 * @param section indicates section value.
	 * @return 0 on success.
	 *         -1 on failure.
	 */
	int getSectionText(
		const std::string& name,
		std::string & sectionText
	);

	/**
	 * Used to Read config Item.
	 * @param section indicates config section.
	 * @param key indicates config item key.
	 * @param defaultvalue indicates value which are
	 *      are supplied by user as default value, if
	 *      fail to get value.
	 * @param itemvalue  indicates value which get from 
	 *       config file.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadItem(
		const std::string& section,
		const std::string& key,
		const std::string& defaultvalue,
		std::string& itemvalue
	);

	/**
	 * Used to Read config Item.
	 * @param section indicates config section.
	 * @param key indicates config item key.
	 * @param defaultvalue indicates value which are
	 *      are supplied by user as default value, if
	 *      fail to get value.
	 * @param itemvalue  indicates value which get from 
	 *       config file.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadItem(
		const std::string& section,
		const std::string& key,
		int defaultvalue,
		int& itemvalue
	);
	
	/**
	 * Used to Read config Item.
	 * @param section indicates config section.
	 * @param key indicates config item key.
	 * @param defaultvalue indicates value which are
	 *      are supplied by user as default value, if
	 *      fail to get value.
	 * @param itemvalue  indicates value which get from 
	 *       config file.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadItem(
		const std::string& section,
		const std::string& key,
		unsigned int defaultvalue,
		unsigned int& itemvalue
	);

	/**
	 * Used to Read config Item, not remove trailing comment
	 *
	 * @param section indicates config section.
	 * @param key indicates config item key.
	 * @param defaultvalue indicates value which are
	 *      are supplied by user as default value, if
	 *      fail to get value.
	 * @param itemvalue  indicates value which get from 
	 *       config file.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadRawItem(
		const std::string& section,
		const std::string& key,
		const std::string& defaultvalue,
		std::string& itemvalue
	);

	/**
	 * Used to Read config Item, not remove trailing comment
	 *
	 * @param section indicates config section.
	 * @param key indicates config item key.
	 * @param defaultvalue indicates value which are supplied by user as default value, if fail to get value.
	 * @param itemvalue  indicates value which get from 
	 *       config file.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadRawItem(
		const std::string& section,
		const std::string& key,
		int defaultvalue,
		int& itemvalue
	);

	/**
	 * Used to Read un-sectioned config Item.
	 *
	 * @param key indicates config item key.
	 * @param itemvalue  indicates value which get from config file.
	 * @param defaultvalue indicates value which are supplied by user as default value, if fail to get value.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadItem(
		const std::string& key,
		const std::string& defaultvalue,
		std::string& itemvalue
	);

	/**
	 * Used to Read un-sectioned config Item.
	 *
	 * @param key indicates config item key.
	 * @param itemvalue  indicates value which get from config file.
	 * @param defaultvalue indicates value which are supplied by user as default value, if fail to get value.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadItem(
		const std::string& key,
		int defaultvalue,
		int& itemvalue
	);
	
	/**
	 *
	 * new interface addby junechen
	 * Used to Read un-sectioned config Item.
	 *
	 * @param key indicates config item key.
	 * @param itemvalue  indicates value which get from config file.
	 * @param defaultvalue indicates value which are supplied by user as default value, if fail to get value.
	 * @return 0 on success. 
	 *        -1 on failure.
	 */
	int ReadItem(
		const std::string& key,
		unsigned int defaultvalue,
		unsigned int & itemvalue
	);


	/** 
	 * Used to write config item.
	 * @param section indicates  config section.
	 * @param key config item key.
	 * @param value config item value.
	 * @return 0 on success.
	 *        -1 on failure.
	 */
	int WriteItem(
		const std::string& section,
		const std::string& key,
		const std::string& value
	);

	/** 
	 * Only add the item in the memory, so you can add some items,
	 * then call WriteFile once to save all the new items into the file.
	 *
	 * @param section indicates  config section.
	 * @param key config item key.
	 * @param value config item value.
	 * @return 0 on success.
	 *        -1 on failure.
	 */
	int AddItem(
		const std::string& section,
		const std::string& key,
		const std::string& value
	);

	/**
	 * This function is used to load
	 * config file to buf.
	 * @return 0 on success.
	 *         -1 on failure.
	 */
	int Init(void);

	/**
	 * This function is used to dump
	 * iternal config information.
	 */
	void dumpinfo(void);

	/**
	 * This function is used to 
	 * trim \t empty string at string 
	 * start or end. 
	 * @return 0 on success.
	 *        -1 on failure.
	 */
	static int TrimString ( std::string & str );

	static int TrimCStr( char * src );

	static void Split(
		const std::string& str,
		const std::string& delim,
		std::vector<std::string>& output
	);
 		
	std::string GetConfigContent(void);
	
private:

	/**
	 * This function is used to 
	 * write back config information 
	 * to physical file.
	 * @return 0  on success.
	 *         -1 on failure.
	 */
	int WriteFile(void);

	/**
	 * This function is used to 
	 * load config information 
	 * @return 0 on success.
	 *         -1 on failure.
	 */
	int LoadFile(void);
  
	const char * GetBuffer();

	/**
	 * find the begin of the specify key
	 *
	 * @param src : file content
	 * @param section : section name
	 * @param key : key name
	 *
	 * @param size : the length of the line
	 *        size > 0 : the key is existence
	 *        size = 0 : the key is not existence
	 *
	 * @return (*size) > 0 : the begin of the line
	 * @return (*size) = 0 : the begin for insert new key
	 *
	 */
	static const char * GetItemPos(
		const char * src,
		const char * section,
		const char * key,
		int * size
	);

	//config file name.
	std::string m_config; 
 
	typedef std::map<std::string,std::string> ConfigType;

	// config section defination.
	std::map<std::string,ConfigType> m_section;  
	
	//std::map<std::string, ConfigType> m_mp_section_cache;
};

}


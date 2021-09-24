// Config.hh
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <set>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>

#include "iString.h"
#include "iConfig.h"

using namespace std;
#define MAX_LINE_LEN  1024*32

#define CGIMAGIC_UN_SECTIONED "CGIMAGIC-UN-SECTIONED"
#define CGIMAGIC_SECTION_TEXT "CGIMAGIC-SECTION-TEXT"

#define CGIMAGIC_SECTION_BUFFER "CGIMAGIC-SECTION-BUFFER"
#define CGIMAGIC_KEY_BUFFER     "CGIMAGIC-KEY-BUFFER"

namespace Comm{

//static set<string> g_setConfig;

int MakeDir(const char *sDir)
{
	if(access(sDir, F_OK) != 0)
	{
		if(mkdir(sDir, 0766) != 0)
		{
			return(1);
		}
	}

	return(0);
}



int ConfigRead( CConfig * config, ConfigItemInfo_t * infoArray )
{
	int ret = 0;

	for( int i = 0; ; ++i )
	{
		ConfigItemInfo_t item = infoArray[i];

		if( NULL == item.pvAddr ) break;

		string value;

		int readRet = 0;

		if( '\0' == item.sSection[0] )
		{
			readRet = config->ReadItem( item.sKey, "", value );
		} else {
			readRet = config->ReadItem( item.sSection, item.sKey, "", value );
		}

		if( 0 != readRet )
		{
			ret = -1;
			//LogInternal( "ConfigRead: [%s]%s is not exist",item.sSection, item.sKey );
		}

		if( CONFIG_TYPE_STR == item.iType )
		{
			memset( item.pvAddr, 0, item.iSize );
			value.copy( (char*)item.pvAddr, item.iSize - 1 );
		} else if( CONFIG_TYPE_INT == item.iType ) {
			*(int*)item.pvAddr = atoi( value.c_str() );
		} else if( CONFIG_TYPE_UIN == item.iType ) {
			*(unsigned int*)item.pvAddr = strtoul( value.c_str(), NULL, 10 );
		} else if( CONFIG_TYPE_HEX == item.iType ) {
			*(unsigned int*)item.pvAddr = strtoul( value.c_str(), NULL, 16 );
		} else {
			LOGERR( "ConfigRead: unknown type %d, [%s]%s",item.iType, item.sSection, item.sKey );
		}
	}

	return ret;
}

void ConfigDump( ConfigItemInfo_t * infoArray )
{
	map< string, vector<ConfigItemInfo_t> > dumpMap;

	for( int i = 0; ; ++i )
	{
		ConfigItemInfo_t item = infoArray[i];

		if( NULL == item.pvAddr ) break;

		dumpMap[ item.sSection ].push_back( item );
	}

	map< string, vector<ConfigItemInfo_t> >::iterator iter = dumpMap.begin();

	for( ; dumpMap.end() != iter; ++iter )
	{
		//Log(COMM_LOG_IN, "INIT: [%s]", iter->first.c_str() );

		vector<ConfigItemInfo_t> & list = iter->second;

		for( unsigned int i = 0; i < list.size(); ++i )
		{
			ConfigItemInfo_t & item = list[i];

			if( CONFIG_TYPE_STR == item.iType )
			{
				//Log(COMM_LOG_IN, "INIT: %s = %s",item.sKey, (char*)item.pvAddr );
			}

			if( CONFIG_TYPE_INT == item.iType )
			{
				//Log(COMM_LOG_IN, "INIT: %s = %d",item.sKey, *(int*)item.pvAddr );
			}

			if( CONFIG_TYPE_UIN == item.iType )
			{
				//Log(COMM_LOG_IN, "INIT: %s = %u",item.sKey, *(unsigned int*)item.pvAddr );
			}
		}
	}
}

CConfig::CConfig( const std::string &configfile)
	:m_config(configfile)
{
}

CConfig & CConfig::operator= (const CConfig& file)
{
	this->m_config = file.m_config;
	this->m_section = file.m_section;
	return *this;
}

const string & CConfig :: getConfigFile()
{
	return m_config;
}

int CConfig::Init(void)
{
	return LoadFile();
}

CConfig::~CConfig(void)
{
}

int CConfig::LoadFile(void)
{
	m_section.clear();

	FILE * fp = fopen( m_config.c_str(), "r" );

	if( NULL != fp )
	{
		struct stat fileStat;
		if( 0 == fstat( fileno( fp ), &fileStat ) )
		{
			LOGERR(  "CCONFIG LoadFile(%s) OK.", m_config.c_str() );
					
			char * tmp = (char*)malloc( fileStat.st_size + 64 );

			// TRICK: add CGIMAGIC_UN_SECTIONED at the begin
			int padLen = snprintf( tmp, 64, "\n[%s]\n", CGIMAGIC_UN_SECTIONED );

			fread( tmp + padLen, fileStat.st_size, 1, fp );
			tmp[ fileStat.st_size + padLen ] = '\0';

			m_section[ CGIMAGIC_SECTION_BUFFER ] [ CGIMAGIC_KEY_BUFFER ] = tmp;
			free( tmp );
		} else {
			LOGERR(  "MAGIC_CONFIG fstat(%s) fail, errno %d, %s",m_config.c_str(), errno, strerror( errno ) );
		}

		fclose( fp ), fp = NULL;
	} else {
		//ErrLog( "MAGIC_CONFIG fopen(%s) fail, errno %d, %s",
				//m_config.c_str(), errno, strerror( errno ) );
	}

	return m_section.size() > 0 ? 0 : -1;
}

const char * CConfig :: GetBuffer()
{
	string & buff = m_section[ CGIMAGIC_SECTION_BUFFER ] [ CGIMAGIC_KEY_BUFFER ];
	return (char*)buff.c_str();
}

std::string CConfig::GetConfigContent(void)
{
	return GetBuffer();
}

int CConfig::getSection( const std::string& name,
		std::map<std::string,std::string>& section )
{
	const char * src = GetBuffer();

	char tmpSection[ 128 ] = { 0 };
	snprintf( tmpSection, sizeof( tmpSection ), "\n[%s]", name.c_str() );

	char line[ 1024 ] = { 0 };

	const char * pos = strstr( src, tmpSection );
	if( NULL != pos ) pos++;

	for( ; NULL != pos; )
	{
		pos = strchr( pos, '\n' );

		if( NULL == pos ) break;
		pos++;

		if( '[' == *pos ) break;

		if( ';' == *pos || '#' == *pos ) continue;

		strncpy( line, pos, sizeof( line ) - 1 );

		char * tmpPos = strchr( line, '\n' );
		if( NULL != tmpPos ) *tmpPos = '\0';

		char * value = strchr( line, '=' );
		if( NULL != value )
		{
			*value = '\0';
			value++;

			TrimCStr( line );
			TrimCStr( value );

			section[ line ] = value;
		}
	}

	return NULL != strstr( src, tmpSection ) ? 0 : -1;
}

int CConfig :: getSectionText( const std::string& name,
		std::string & sectionText )
{
	const char * src = GetBuffer();

	char tmpSection[ 128 ] = { 0 };
	snprintf( tmpSection, sizeof( tmpSection ), "\n[%s]", name.c_str() );

	const char * pos = strstr( src, tmpSection );
	if( NULL != pos )
	{
		pos = strchr( pos + 1, '\n' );
		if( NULL != pos )
		{
			const char * endPos = strstr( pos, "\n[" );
			if( NULL == endPos ) endPos = strchr( pos, '\0' );

			if( endPos > pos ) sectionText.append( pos + 1, endPos - pos - 1 );
		}
	}

	return NULL != pos ? 0 : -1;
}

int CConfig::TrimString ( string & str )
{
	string drop = " \r\n";
	// Trim right
	str  = str.erase( str.find_last_not_of(drop)+1 );
	// Trim left
	str.erase( 0, str.find_first_not_of(drop) );

	return 0;
}

int CConfig :: TrimCStr( char * asString )
{
	register int liLen ;
	register char *lp ;

	liLen = strlen ( asString ) ;
	while ( liLen > 0 && isspace( asString [liLen - 1] ) )
		liLen -- ;
	asString [liLen] = '\0' ;
	for ( lp = asString; isspace(*lp); lp ++ ) ;
	if ( lp != asString )
		memmove ( asString, lp, liLen + 1 ) ;

	return 0;
}

void CConfig :: Split(const std::string& str, const std::string& delim,
		std::vector<std::string>& output)
{
	unsigned int offset = 0;

#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)
	unsigned long long  delimIndex = 0;
#else
	unsigned int delimIndex = 0;
#endif

	delimIndex = str.find(delim, offset);

	while (delimIndex != string::npos)
	{
		output.push_back(str.substr(offset, delimIndex - offset));
		offset += delimIndex - offset + delim.length();
		delimIndex = str.find(delim, offset);
	}

	output.push_back(str.substr(offset));
}

int CConfig::getSectionList( std::vector<std::string>& sectionlist )
{
	const char * src = GetBuffer();

	for( ; ; )
	{
		const char * pos1 = strstr( src, "\n[" );

		if( NULL == pos1 ) break;

		char section[ 128 ] = { 0 };
		strncpy( section, pos1 + 2, sizeof( section ) - 1 );

		char * pos2 = strchr( section, ']' );
		if( pos2 != NULL ) *pos2 = '\0';

		sectionlist.push_back( section );

		src = pos1 + 2;
	}

	return 0;       
}

void CConfig::dumpinfo(void)
{
	vector<string> sectionList;

	getSectionList( sectionList );

	for( vector<string>::iterator iter = sectionList.begin(); sectionList.end() != iter; ++iter )
	{
		map< string, string > keyValMap;

		cout << "[" << *iter << "]" << endl;

		getSection( *iter, keyValMap );

		for( map< string, string >::iterator mit = keyValMap.begin(); keyValMap.end() != mit; ++mit )
		{
			cout << mit->first << " = " << mit->second << endl;
		}
	}
}

int CConfig::WriteFile(void)
{
	const char * src = GetBuffer();

	fstream outstr;
	outstr.open(m_config.c_str(),fstream::out);
	if(outstr.fail()) return -1;

	// TRICK: skip CGIMAGIC_UN_SECTIONED at the begin
	char tmp[ 128 ] = { 0 };
	int padLen = snprintf( tmp, sizeof( tmp ), "\n[%s]\n", CGIMAGIC_UN_SECTIONED );

	outstr.write( src + padLen, strlen( src + padLen ) );

	if(outstr.fail())
	{
		outstr.close();
		return -1;
	}
 
	outstr.close();

	return 0;
}

int CConfig::WriteItem( const std::string& section,
		const std::string& key, const std::string& value )
{
	AddItem( section, key, value );

	return WriteFile();
}

int CConfig :: AddItem( const std::string& section,
		const std::string& key, const std::string& value )
{
	int size = 0;

	const char * src = GetBuffer();
	const char * pos = GetItemPos( src, section.c_str(), key.c_str(), &size );

	string newBuffer;

	if( NULL != pos )
	{
		newBuffer.append( src, pos - src );
		newBuffer.append( key ).append( " = " ).append( value );
		if( size <= 0 ) newBuffer.append( "\n" );
		newBuffer.append( pos + size );
	} else {
		newBuffer.append( src );
		newBuffer.append( "\n[" ).append( section ).append( "]\n" );
		newBuffer.append( key ).append( " = " ).append( value ).append( "\n" );
	}

	m_section[ CGIMAGIC_SECTION_BUFFER ] [ CGIMAGIC_KEY_BUFFER ] = newBuffer;

	return 0;
}

static int cpIni_GetString(const char * src, int srclen,
		const char *sSection, const char *sItem, const char *sDefault,
		char *sValue, const int nValueLen)
{
	char	*pSection = NULL, *pNextSection = NULL;
	char	*pItem = NULL, *pValue = NULL, *pTestSection;
	char	*pTemp;
	int		n, nPos, nSectionLen, nItemLen;

	strncpy(sValue, sDefault, nValueLen);
	nSectionLen = strlen(sSection);
	nItemLen = strlen(sItem);

	char * m_sBuffer = (char*)src;
	int m_nSize = srclen;

	// locate section first
	pTemp = m_sBuffer;
	while(pTemp < m_sBuffer + m_nSize)
	{
		pSection = strcasestr(pTemp, sSection);
		if(pSection == NULL)
		{
			return(1);
		}

		// section name at the beginning of ini file, no '['.
		if(pSection == m_sBuffer)
		{
			return(1);
		}

		if(*(pSection-1) != '[' || *(pSection+nSectionLen) != ']')
		{
			pTemp = pSection + nSectionLen;
			continue;		
		}
		// make sure the section name is at the beginning of the line
		if(pSection > m_sBuffer + 1)
		{
			if(*(pSection-2) != '\n')
			{
				pTemp = pSection + nSectionLen;
				continue;		
			}
		}

		pTemp = pSection;
		break;
	}

	// locate the entry of next section
	pTestSection = pTemp;
	while(pTestSection != NULL)
	{
		pNextSection = strstr(pTestSection, "[");
		if(pNextSection != NULL)
		{
			// '[' must at the first character of the line
			if(*(pNextSection-1) == '\n')
			{
				break;
			}
			else
			{
				// maybe '[' is at the comment or somewhere else, search backward
				pTestSection = pNextSection + 1;
			}
		}	
		else
		{
			break;
		}
	}
	// no more section, assign the end of the buffer to pNextSection
	if(pNextSection == NULL)
	{
		pNextSection = m_sBuffer + m_nSize;
	}

	// locate item
	while(pTemp < m_sBuffer + m_nSize)
	{
		pItem = strcasestr(pTemp, sItem);
		if(pItem == NULL)
		{
			return(1);
		}

		// no matched item in the section, this is the item belonging to following section
		if(pItem >= pNextSection)
		{
			return(1);
		}

		// not the begining of the line
		if(*(pItem-1) != '\n')
		{
			pTemp = pItem + nItemLen;
			continue;
		}
		// not the exactly matched word
		if(*(pItem+nItemLen) != ' ' && *(pItem+nItemLen) != '=' && *(pItem+nItemLen) != '\t')
		{
			pTemp = pItem + nItemLen;
			continue;
		}
	
		// extract value
		pValue = strstr(pItem, "=");
		if(pValue == NULL)
		{
			return(1);
		}
		// skip '='
		pValue++;

		// the end of file
		if(pValue >= m_sBuffer + m_nSize)
		{
			return(1);
		}

		// copy the value
		nPos = 0;
		// reserve one byte to store NULL
		for(n=0; nPos<nValueLen-1 && pValue[n] != '\r' && pValue[n] != '\n' && pValue[n] != 0; ++n)
		{
			// skip left space or tab
			if((pValue[n] == ' ' || pValue[n] == '\t') && nPos == 0)
				continue;
			// skip comment
			if(pValue[n] == ';')
			{
				if(n > 0)
				{
					// skip the second ';'
					if(pValue[n+1] == ';')
					{
						sValue[nPos] = pValue[n];
						nPos++; 
						n++;
						continue;
					}
				}
				break;
			}

			sValue[nPos] = pValue[n];
			nPos++;
		}
		sValue[nPos] = 0;
		break;
	}

	return(0);
}

int CConfig::ReadItem( const std::string& section,
		const std::string& key, const std::string& defaultvalue,
		std::string& itemvalue )
{
	if( section.size() <= 0 || key.size() <= 0 ) return -1;

#if 1
	string tmpsec,tmpkey;
	tmpsec = section;
	StrLower(tmpsec);
	tmpkey = key;StrLower(tmpkey);
	map<std::string, ConfigType>::iterator itsec = m_section.find( tmpsec );
	if (itsec != m_section.end())
	{
		ConfigType::iterator itkey = itsec->second.find( tmpkey );
		if (itkey != itsec->second.end())
		{
			itemvalue = itkey->second;
			return 0;
		}
	}
	
	char tmpValue[ 1024 ] = { 0 };

	string & buff = m_section[ CGIMAGIC_SECTION_BUFFER ] [ CGIMAGIC_KEY_BUFFER ];

	int ret = cpIni_GetString( buff.c_str(), buff.size(),
			section.c_str(), key.c_str(), defaultvalue.c_str(),
			tmpValue, sizeof( tmpValue ) );

	TrimCStr( tmpValue );
	itemvalue = tmpValue;
	m_section[tmpsec][tmpkey] = itemvalue;

	return ret == 0 ? 0 : -1;

#else

	int ret = ReadRawItem( section, key, defaultvalue, itemvalue );

	if( 0 == ret )
	{
		int pos = itemvalue.find_first_of( ";" );
		if( string::npos == pos ) pos = itemvalue.find_first_of( "#" );
		if( string::npos != pos ) itemvalue = itemvalue.substr( 0, pos );

		TrimString( itemvalue );
	}

	return ret;

#endif

}

int CConfig::ReadItem( const std::string& section, const std::string& key, int defaultvalue, int& itemvalue )
{
    string sValue;
    string sDefault = StrFormat("%d", defaultvalue);
    int ret = ReadItem(section, key, sDefault, sValue);
    itemvalue = atoi(sValue.c_str());
    return ret;
}

int CConfig::ReadItem( const std::string& section, const std::string& key, 
		unsigned int defaultvalue, unsigned int & itemvalue )
{
    string sValue;
    string sDefault = StrFormat("%u", defaultvalue);
    int ret = ReadItem(section, key, sDefault, sValue);
    itemvalue = (unsigned int)strtoul(sValue.c_str(), NULL, 10);
    return ret;
}

int CConfig::ReadRawItem( const std::string& section,
		const std::string& key, const std::string& defaultvalue,
		std::string& itemvalue )
{
	if( section.size() <= 0 || key.size() <= 0 ) return -1;

	int size = 0;

	const char * src = GetBuffer();
	const char * pos = GetItemPos( src, section.c_str(), key.c_str(), &size );

	if( NULL != pos && size > 0 )
	{
		const char * tmpPos = strchr( pos, '=' );

		if( NULL != tmpPos && ( tmpPos - pos ) < size )
		{
			itemvalue = "";
			itemvalue.append( tmpPos + 1, pos + size - tmpPos - 1 );
			TrimString( itemvalue );
		}
	} else {
		itemvalue = defaultvalue;
	}

	return ( NULL != pos && size > 0 ) ? 0 : -1;
}

int CConfig::ReadRawItem( const std::string& section, const std::string& key, int defaultvalue, int& itemvalue )
{
    string sValue;
    string sDefault = StrFormat("%d", defaultvalue);
    int ret = ReadRawItem(section, key, sDefault, sValue);
    itemvalue = atoi(sValue.c_str());
    return ret;
}

const char * CConfig :: GetItemPos( const char * src,
		const char * section, const char * key, int * size )
{
	const char * ret = NULL;

	*size = 0;

	char tmpSection[ 128 ] = { 0 };
	snprintf( tmpSection, sizeof( tmpSection ), "\n[%s]", section );

	char tmpKey[ 128 ] = { 0 };
	snprintf( tmpKey, sizeof( tmpKey ), "\n%s", key );

	const char * endPos = NULL;
	const char * pos = strcasestr( src, tmpSection );
	if( NULL != pos )
	{
		pos = strchr( pos + 1, '\n' );
		if( NULL == pos ) pos = strchr( src, '\0' );

		endPos = strstr( pos, "\n[" );
		if( NULL == endPos ) endPos = strchr( pos, '\0' );
	}

	for( ; NULL != pos && pos < endPos; )
	{
		pos = strcasestr( pos, tmpKey );

		if( NULL == pos || pos > endPos ) break;

		const char * tmpPos = pos + strlen( tmpKey );
		if( ( !isspace( *tmpPos ) ) && ( '=' != *tmpPos ) ) continue;

		pos++;

		const char * eol = strchr( pos, '\n' );
		if( NULL == eol ) eol = strchr( pos, '\0' );

		tmpPos = strchr( pos, '=' );
		if( NULL != tmpPos && tmpPos < eol )
		{
			ret = pos;
			*size = eol - pos;

			break;
		}
	}

	if( NULL == ret )
	{
		ret = endPos;
		*size = 0;
	}

	return ret;
}

int CConfig :: ReadItem( const std::string& key, const std::string& defaultvalue, std::string& itemvalue )
{
	return ReadItem( CGIMAGIC_UN_SECTIONED, key, defaultvalue, itemvalue );
}

int CConfig::ReadItem(const std::string& key, int defaultvalue, int& itemvalue )
{
    return ReadItem(CGIMAGIC_UN_SECTIONED, key, defaultvalue, itemvalue);
}

int CConfig::ReadItem(const std::string& key, unsigned int defaultvalue, unsigned int & itemvalue )
{
    return ReadItem(CGIMAGIC_UN_SECTIONED, key, defaultvalue, itemvalue);
}


}
//

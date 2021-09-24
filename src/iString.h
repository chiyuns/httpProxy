/**
@file 
@brief 字符串相关函数
*/

#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <algorithm>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <set>
#include <errno.h>

using namespace std;

//typedef std::vector<string> TStringList;
//typedef std::string TString;
/// Comm库
namespace Comm
{
	typedef std::vector<string> TStringList;
	typedef std::string TString;
/// Comm库
	enum TECharSet{csGBK,csUTF8,csGB18030};

	/**
	 * @brief 查找目标字符在源字符串中出现的位置
	 * @param pcSrc 源字符串
	 * @param cFind 查找字符
	 * @param nStart 在源字符串中的开始位置
	 * @param bCaseSensitive 是否大小写敏感
	 * @retval 查找到的位置，-1表示没找到
	 */
	int StrFind(const char* pcSrc, char cFind, int nStart = 0, bool bCaseSensitive = true);

	/**
	 * @brief 查找目标字符串在源字符串中出现的位置
	 * @param pcSrc 源字符串
	 * @param pcFind 查找字符串
	 * @param nStart 在源字符串中的开始位置
	 * @param bCaseSensitive 是否大小写敏感
	 * @retval 查找到的位置，-1表示没找到
	 */
	int StrFind(const char* pcSrc, const char* pcFind, int nStart = 0, bool bCaseSensitive = true);

	/**
	 * @brief 查找目标字符串中的任意字符在源字符串中首先出现的位置，例如pcSrc="abcd", pcFind="cb", 那么则返回1（b首先出现在位置1）
	 * @param pcSrc 源字符串
	 * @param pcFind 查找字符串
	 * @param nStart 在源字符串中的开始位置
	 * @param bCaseSensitive 是否大小写敏感
	 * @retval 查找到的位置，-1表示没找到
	 */
	int StrFindFirstOf(const char* pcSrc, const char* pcFind, int nStart = 0, bool bCaseSensitive = true);

	/**
	 * @brief 去掉字符串前后中出现的字符。
	 * @param pcSrc 源字符串
	 * @param pcTrim 需要去掉的字符列表，默认为' ','\\t','\\r','\\n'。
	 */
	void StrTrim(char* pcSrc, const char* pcTrim = " \t\r\n");

	/**
	 * @brief 去掉字符串前后中出现的字符。
	 * @param[in,out] sSrc 源字符串
	 * @param sTrim 需要去掉的字符列表，默认为' ','\\t','\\r','\\n'。
	 */
	void StrTrim(std::string& sSrc, const std::string& sTrim = " \t\r\n");

	/**
	 * @brief 把字符串中的字母转换为大写。
	 * @param[in,out] pcSrc 源字符串
	 */
	void StrUpper(char* pcSrc);
	
	/**
 	 * @brief 把字符串中的字母转换为大写。
	 * @param[in,out] sSrc 源字符串
	 */
	void StrUpper(std::string& sSrc);	
	
	/**
	 * @brief 把字符串中的字母转换为小写。
	 * @param[in,out] pcSrc 源字符串
	 */
	void StrLower(char* pcSrc);
	
	/**
 	 * @brief 把字符串中的字母转换为小写。
	 * @param[in,out] sSrc 源字符串
	 */
	void StrLower(std::string& sSrc);

	/**
	 * @brief 字符串copy。
	 * @param[out] pcDst 目标字符串，会在结尾补\\0。
	 * @param pcSrc 源字符串。
	 * @param nDstLen 目标字符串允许的长度(实际copy的长度为nDstLen-1，最后一个字符为\\0)。
	 */
	void StrnCpy(char* pcDst, const char* pcSrc, int nDstLen);

	/**
	 * @brief 字符串串接
	 * @param[out] pcDst 目标字符串，会在结尾补\\0。
	 * @param pcSrc 源字符串。
	 * @param nDstLen 目标字符串允许的长度(实际copy的长度为nDstLen-1，最后一个字符为\\0)。
	 * @retval 目的串
	 */
	char* StrnCat(char* pcDst, const char* pcSrc, int nDstLen);

	/**
	 * @brief 字符串比较。
	 * @param pcStr1 字符串1。
	 * @param pcStr2 字符串2。
	 * @param bCaseSensitive 是否大小写敏感。
	 * @retval <0表示pcStr1<pcStr2; ==0表示pcStr1==pcStr2; >0表示pcStr1>pcStr2。
	 */
	int StrCmp(const char* pcStr1, const char* pcStr2, bool bCaseSensitive = true);

	/**
	 * @brief 字符串比较（指定比较的长度）。
	 * @param pcStr1 字符串1。
	 * @param pcStr2 字符串2。
	 * @param nLen 需要比较的长度
	 * @param bCaseSensitive 是否大小写敏感。
	 * @retval <0表示pcStr1<pcStr2; ==0表示pcStr1==pcStr2; >0表示pcStr1>pcStr2。
	 */
	int StrnCmp(const char* pcStr1, const char* pcStr2, int nLen, bool bCaseSensitive = true);

	/**
	 * @brief 字符串是否相等。
	 * @param pcStr1 字符串1。
	 * @param pcStr2 字符串2。
	 * @param bCaseSensitive 是否大小写敏感。
	 * @retval 是否相等。
	 */
	bool StrEqual(const char* pcStr1, const char* pcStr2, bool bCaseSensitive = true);

	/*!
	 * \brief Compare two strings for equality, ignoring case.
	 * 
	 * For case-sensitive comparison, use (s1 == s2);
	 * \param s1 The first string to compare
	 * \param s2 The second string to compare
	 * \return \c true if the strings are equal, \c false if they are not
	 */
	bool stringsAreEqual(const std::string& s1,const std::string& s2);
	    
	/*!
	 * \brief Compare two strings for equality, ignoring case.
     *
	 * For case-sensitive comparison, use (s1 == s2);
	 * \param s1 The first string to compare
	 * \param s2 The second string to compare
	 * \param n The number of characters to compare.
	 * \return \c true if the strings are equal, \c false if they are not
	 */
	bool stringsAreEqual(const std::string& s1,const std::string& s2,size_t n); 

	/**
	 * @brief 根据分隔符分割字符串，返回分割后的字符串列表。
	 * @param pcStr 源字符串
	 * @param pcDelimiter 分隔符串
	 * @param bFullCheck 表示delimiter是完全匹配还是只匹配里面的某一个字符
	 * @param[out] vecResult 分割后的字符串列表
	 */
	void StrSplitList(const char* pcStr, const char* pcDelimiter, bool bFullCheck, std::vector<std::string>& vecResult);

	/// 根据分隔符分割字符串，支持""内的字符串不进行分割。（原stroper.hpp中的gpcParseString）
	/**
	 * 例：()				-> NULL NULL  			\n
	 *     ("")				-> () NULL  			\n
	 *     (abc)			-> (abc) NULL  			\n
	 *     (a b c)			-> (a) (b) (c) NULL  	\n
	 *     ("a b" c)		-> (a b) (c) NULL  		\n
	 *     ("a b"c d)   	-> (a bc) (d) NULL  	\n
	 *     ("\"a b\""c d)	-> ("a b"c) (d) NULL	\n
	 * @param[out] pcResult 结果字符串，分割后的字符串存放在这里。
	 * @param nResultLen 结果字符串的长度
	 * @param pcSrc 源字符串 
	 * @param nIndex 取分割后的第几个字符串，从0开始。
	 * @param[out] ppcNext 当前分割后的下一个字符位置
	 * @param cDelimiter 分隔符，默认为' '或'\\t'或'\\r'或'\\n'
	 * @retval 返回结果字符串位置
	 */
	const char* StrQuotedSplit(char* pcResult, int nResultLen, const char* pcSrc, int nIndex, 
			const char** ppcNext = NULL, char cDelimiter = 0);
	
	/**
	 * @brief 把被左和右字符括住的字符串位置返回
	 * @param pcStr 源字符串
	 * @param cLeftEnclose 左括字符
	 * @param cRightEnclose 右括字符
	 * @param[out] pnLen 括住字符串的长度
	 * @retval 括住的字符串在pcStr中的地址，NULL表示没有对应的字符串。
	 */
	const char* StrEnclose(const char* pcStr, char cLeftEnclose, char cRightEnclose, int* pnLen);
	
	/**
	 * @brief 把被左和右字符括住的字符串位置返回
	 * @param pcStr 源字符串
	 * @param cLeftEnclose 左括字符
	 * @param cRightEnclose 右括字符
	 * @retval 括住的字符串
	 */
	std::string StrEnclose(const char* pcStr, char cLeftEnclose, char cRightEnclose);

	/** 
	 * @brief Verfiy if the content of string is integer.
	 * @param sString: Input string.
	 * @return 0=OK, 1=FAIL
	 */
	int StrIsInteger(const char *sString);

	/**
	 * @brief 格式化字符串
	 * @param pcFmt 格式串
	 * @retval 格式化后的字符串
	 */
	std::string StrFormat(const char* pcFmt, ...);

	/**
	 * @brief 格式化字符串
	 * @param sStr 格式化后的字符串
	 * @param pcFmt 格式串
	 */
	void StrFormat(std::string& sStr, const char* pcFmt, ...);

	  /*!
 	   * \brief Extract a substring contained within two separators.
	   * 
	   * For example, after the call
	   * \code
	   * std::string data = "11foo22";
	   * std::string res;
	   * res = extractBetween(data, "11", "22");
	   * \endcode
	   * \c res will be "foo".
	   * \param data The data to search.
	   * \param separator1 The first logical separator.
	   * \param separator2 The second logical separator.
	   * \return The substring between the separators.
	   */
	std::string extractBetween(const std::string& data,const std::string& separator1,const std::string& separator2);
	                   
	char * extractBetween(char *pData,char* separator1,char* separator2);

	char* MemSearch(char *pDst, char *pPat, int nDstLen, int nPatLen);
	const char * ParseString ( register char *asResult, int aiResultSize, 
		const char *asSource,  int aiIndex, const char ** lpcNext = (const char **)0, char acDelimitor = 0 ) ;

	int IsSpace ( register int c ) ;
	int IsAlnum ( register int c ) ;
	int IsDigit ( register int c ) ;
	int IsAlpha ( register int c ) ;

	int StringToBitmap(const char *asString,char *pBitsBuff,size_t nBuffSize);
	string BitmapToString(char *pBitsBuff,size_t nBitCount);

	void StrReplaceAll( string & haystack, string needle, string s );

	unsigned long int StringHash(
		register char *k,            /* the key */
		register unsigned long  int length,   /* the length of the key */
		register unsigned long  int initval   /* the previous hash, or an arbitrary value */
	);

	string Left(const string & sStr,int nCount);

	string Right(const string &sStr, int nCount);

	string AllTrim(const string &sStr,const string & sToTrim=" ");

	string LeftTrim(const string &sStr,const string & sToTrim=" ");

	string RightTrim(const string &sStr,const string & sToTrim=" ");

	int StrToInt(const string &sStr);

	unsigned int StrToUInt(const string &sStr);

	long long int StrToLLInt(const string &sStr);

	unsigned int BufferToUInt(const char *psz);

	string Mid(const string & sStr,long lBegin);

	string Mid(const string & sStr,long lBegin,long lCount );

	string IntToStr(const int i);

	string UIntToStr(const unsigned int i);

	string LLIntToStr(const long long int i);

	string Padl(const string & sStr,unsigned long lCount,char ch);

	int SeparateText(vector<string> &slStringList,const string &sStr,const char cSeparator);

//==========================================================================
// 函数 : BCBToStr
// 用途 : 将BCB码转换为数字字符串编码
// 原型 : TCString BCBToStr(const TCString & sBCBStr);
// 参数 : BCB串
// 返回 : 数字字符串
// 说明 :
//    原串: "\x13\x29\x38\x76"
//    结果: "13293876"
//==========================================================================
	int BCBToStr(const char * szBCBStr,char * szStr);

//==========================================================================
// 函数 : StrToBCB
// 用途 : 将数字字符串转换为BCB码
// 原型 : TCString StrToBCB(const TCString & sNumStr);
// 参数 : 数字字符串
// 返回 : BCB编码字符串
// 说明 :
//    原串: "13293876"
//    结果: "\x13\x29\x38\x76"
//==========================================================================
	int StrToBCB(const char * szNumStr,char * szStr);

//字符串匹配函数 limq 2005-4-13
//可以匹配*?为通配符的字符串:*表示0或多个任意字符,?表示1个任意字符
//按照最长字串匹配算法,可以支持相连的两或多个**号,*?等,目前为止,我暂时没有发现有什么局限性.
	bool FilterStr(const string &sStr,const string &sFilter);
	bool FilterStr(const char  * const str,const char * const filter);

	bool IsUpperAlphabetString(const string & sStr);

	bool IsNumber(const string & sStr);

	bool IsNumber(const char * psz);

	//是否浮点数，整数也是是浮点数
	bool IsFloat(const string & sStr);

	bool IsFloat(const char * psz);

	string Replace(const string& sStr,const string sToFind,const string &sReplaceWith);

	bool IsAsciiStr(const string & sStr);

	string NumericCharacterEntities(string sStr, const string sSearchWord, const int nBase, char cEndChar='\0');

	string ToLowerCase(const string & sSrc);

	string ToUpperCase(const string & sSrc);

	string GetSeperatedTextByIndex(const string &sStr,const int nIndex,const string & sSeparatorList);

	string GetNext(const string & sStr,size_t & nStartPos,const TECharSet emCharSet);

}


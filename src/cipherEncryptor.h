#ifndef CIPHER_ENCRYPTOR_H_
#define CIPHER_ENCRYPTOR_H_

#include<stdio.h>
#include <string>

std::string GenerPasswd();


//data 加密
char * base64Encode(const char *buffer, int length, bool newLine);
char* pwdEncrptor(unsigned char* userKey, const char* pwd);

/////////////////////////////////////////////////////////////////////////////////////
char * base64Decode(const char *input, int length, bool newLine);
char* pwdDecrptor(unsigned char* userKey, const char* encryptPwd);

std::string EncryptPass(std::string src);
std::string DecryptPass(std::string src);

bool IsBase64(std::string &ss);
#endif

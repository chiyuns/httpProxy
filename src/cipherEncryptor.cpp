#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <string.h>
#include <string>
#include "cipherEncryptor.h"
using std::string;

char* pwdEncrptor(unsigned char* userKey, const char* pwd) {
    char fn[] = "[pwdEncrptor]";
    //首先将pwd进行md5加密
    unsigned char MD5result[17] = {'\0'};
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, pwd, strlen(pwd));
    MD5_Final(MD5result, &md5_ctx);

    int len = 0;
    char md5[17] = {'\0'};
    for(int i = 4; i < 12; ++i) {
        len += snprintf(md5 + len, sizeof(md5) - len, "%02x", MD5result[i]);
    }

    //组装要加密的字符串，密码加签名
    int pwdSignLen = strlen(pwd) + 16;
    const int buflen = (pwdSignLen + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE * AES_BLOCK_SIZE;
    unsigned char *data = (unsigned char *)malloc(buflen);
    memset(data, '\0', buflen);
    snprintf((char*)data, buflen, "%s%s", pwd, (const char*)&md5);
//	printf("pwdSign is %s\n", data);

    //进行加密
    unsigned char *encrypto = (unsigned char *)malloc(buflen);
    memset((void*)encrypto, 0, buflen);

    //设置加密key及秘钥长度
    //理论上AES加密是根据S盒进行字节替换的，因此原文和密文个字节一一对应大小相同
    AES_KEY key;
    AES_set_encrypt_key((const unsigned char *)userKey, AES_BLOCK_SIZE * 8, &key);

    len = 0;
    //循环加密， 每次加密长度是AES_BLOCK_SIZE长度的数据
    while (len < buflen) {
        AES_encrypt(data + len, encrypto + len, &key);
        len += AES_BLOCK_SIZE;
    }
//	for(int i = 0 ; i < buflen; ++i)
//	printf("%02x", encrypto[i]);
//	printf("\n");

//	//将结果做base64编码
//	printf("encrypt pwd len for %s is \n%d\n",data, buflen);
    char * encode = base64Encode((const char *)encrypto, buflen, false);
//	printf("encode pwd %s is \n%s\n",pwd, encode);
    return encode;
}




char* pwdDecrptor(unsigned char* userKey, const char* encryptPwd) {
    char fn[] = "[pwdDecrptor]";
//  printf("userkey: %s, len: %d\n", userKey, strlen(userKey));
//  printf("epwd: %s, len: %d\n", encryptPwd, strlen(encryptPwd));

    //首先做解码
    char * encrypto = base64Decode(encryptPwd, strlen(encryptPwd), false);
    if (NULL == encrypto || 0 == strlen(encrypto)) {
        fprintf(stderr, "%sencrypto length is zero!\n", fn);
        return NULL;
    }

    int len = strlen(encryptPwd) - 1;
    while ('\0' == encrypto[len] && '\0' == encrypto[len - 1] ) {
        --len;
    }

    //再做解密
    const int buflen = (len + 1 + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE * AES_BLOCK_SIZE;

    unsigned char *plain = (unsigned char *)malloc(buflen);
    memset((void*)plain, 0, buflen);

    AES_KEY key;
    AES_set_decrypt_key((const unsigned char *)userKey, AES_BLOCK_SIZE * 8, &key);

    len = 0;
    //循环解密
    //每次输入16字节，输出16字节，如果不够需要填充
    while (len < buflen) {
        AES_decrypt((const unsigned char*)encrypto + len, plain + len, &key);
        len += AES_BLOCK_SIZE;
    }
//  printf("plain data len = %zu, text = %s\n", strlen((const char*)plain), plain);
    free(encrypto);


    //提取原始密码
    int pwdLen = strlen((const char*)plain) - 16 ;
    char* pwd = (char *)malloc(pwdLen + 1);
    if (NULL == pwd) {
        fprintf(stderr, "%sFailed to malloc!\n", fn);
        return NULL;
    }
    memset(pwd, '\0', pwdLen + 1);
    memcpy(pwd, plain, pwdLen);
    //printf("pwd is %s\n", pwd);

    //求解pwd的md5值
    unsigned char MD5result[17] = {'\0'};
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, pwd, strlen(pwd));
    MD5_Final(MD5result, &md5_ctx);

    len = 0;
    char md5[17] = {'\0'};
    for(int i = 4; i < 12; ++i) {
        len += snprintf(md5 + len, sizeof(md5) - len, "%02x", MD5result[i]);
    }

    //对比md5值是否一致
    if (memcmp(md5, plain + pwdLen, 16) == 0) {
        //printf("decode success\n");
        free(plain);
        return pwd;
    } else {
        free(plain);
        free(pwd);
        fprintf(stderr, "%sdecode password %s failed\n", fn, encryptPwd);
        return NULL;
    }
}

// base64 编码
char * base64Encode(const char *buffer, int length, bool newLine) {
    char fn[] = "[base64Encode]";
    BIO *bmem = NULL;
    BIO *b64 = NULL;

    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, buffer, length);
    BIO_flush(b64);

    BUF_MEM *bptr;
    BIO_get_mem_ptr(b64, &bptr);

    char *buff = (char *)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;
    
    BUF_MEM_free(bptr);
    BIO_set_close(b64, BIO_NOCLOSE);
    BIO_free_all(b64);
    return buff;
}


char * base64Decode(const char *input, int length, bool newLine) {
    char fn[] = "[base64Decode]";
    if (NULL == input) {
        fprintf(stderr, "%sinput cannot be NULL!\n", fn);
        return NULL;
    }

    BIO *b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    BIO *bmem = NULL;
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);

    char *buffer = (char *)malloc(length);
    if (NULL == buffer) {
        fprintf(stderr, "%sFailed to malloc!\n", fn);
    }
    memset(buffer, 0, length);
    int ret = BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);
    if (ret < 0) {
        std::cerr << fn << "Failed to BIO_read, ret: " << ret << std::endl;
        return NULL;
    }
    return buffer;
}

string GenerPasswd() {
    char key[AES_BLOCK_SIZE + 1] = {'0'};
    int keyLen = 0;
    keyLen += snprintf(key + keyLen, sizeof(key) - keyLen, "Sdj");
    keyLen += snprintf(key + keyLen, sizeof(key) - keyLen, "%c", '&');
    keyLen += snprintf(key + keyLen, sizeof(key) - keyLen, "%d", 123456);
    keyLen += snprintf(key + keyLen, sizeof(key) - keyLen, "%c", '#');
    int i;
    for(i = keyLen; i < 16; i++) { //可自由设置密钥
        key[i] = 32 + i;
    }
    return string(key);
}

std::string EncryptPass(std::string src) {
    string pwd = GenerPasswd();
  //  std::cout << "key: " << pwd << std::endl;
    return pwdEncrptor((unsigned char*)pwd.c_str(), src.c_str());
}

std::string DecryptPass(std::string src) {
    string pwd = GenerPasswd();
   // std::cout << "key: " << pwd << std::endl;
    return pwdDecrptor((unsigned char*)pwd.c_str(), src.c_str());
}

static char base64_code[]  = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
           'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
           'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a',
           'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
           'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
           't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
           '2', '3', '4', '5', '6', '7', '8', '9', '+',
           '/', '='};

bool IsBase64(std::string &ss) 
{
    if ("" == ss) {
        return false;
    }

    if ((ss.size() % 4) != 0) {
        return false;
    }

    for (auto ch : ss){
       if ((ch >= 'A' && ch <= 'Z') || 
           (ch >= 'a' && ch <= 'z') || 
           (ch >= '0' && ch <= '9') ||
            ch == '+' ||
            ch == '/' || 
            ch == '=') {
           continue;
       }
       return false;
    }
    
    return true;
}

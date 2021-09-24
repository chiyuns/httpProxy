#ifndef HTTPPROXYSVR_H__
#define HTTPPROXYSVR_H__

#include "httplib.h"
#include <mutex>
using namespace httplib;

class HttpProxyServer {
 public:
  int Listen(const char *host, int port);

 private:
  void init();
 private:
  Server m_svr;
  std::mutex m_queueMutex;
};



#endif

#pragma once
#include <mutex>
#include <atomic>
#include <memory>
#include <queue>
#include <thread>
#include <functional>
#include <condition_variable>
#include "Connection.h"
struct UserConfig{
    std::string ip;
    int port;
    std::string userName;
    std::string passWd;
    std::string dbName;
    int initSize;
    int maxSize;
    int maxIdleTime;
    int connectTimeout;

};
class ConnPool : public NonCopyable
{
private:
    std::mutex queMutex_;
    std::atomic_int connnectionCnt_;
    UserConfig userConfig_;
    std::condition_variable  cv_;
    std::queue<Connection*> connQue_;
private:
    void produceConnectionTask();
    void scannerConnectionTask();
public:
    ConnPool();
    static ConnPool* getConnPool();
    static bool getConfigFile(UserConfig& UserConfig);
    std::shared_ptr<Connection> getConnection();
};
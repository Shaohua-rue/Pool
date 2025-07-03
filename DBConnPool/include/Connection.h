#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include "mysql/mysql.h"

using TimePoint = std::chrono::system_clock::time_point;
using Duration = std::chrono::system_clock::duration;

void Log(std::string str);

class NonCopyable
{
public:
    NonCopyable(const NonCopyable& noncopyable) = delete;
    NonCopyable& operator=(const NonCopyable& noncopyable) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};
class Connection : public NonCopyable 
{
public:
    Connection();
    ~Connection();

    bool connect(std::string ip, unsigned short port,std::string user, std::string password, std::string dbname);

    bool update(std::string sql);
    MYSQL_RES* query(std::string sql);

    void refreshAliveTime(){alivetime_ = std::chrono::system_clock::now();}
    Duration getAliveTime() const {
        return std::chrono::system_clock::now() - alivetime_;
    }


private:
    MYSQL* conn_;
    TimePoint alivetime_;
};


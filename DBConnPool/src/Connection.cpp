#include "Connection.h"

void Log(std::string str){
    std::cout << __FILE__ << ":" << __LINE__ << " " << \
	__TIMESTAMP__ << " : " << str << std::endl;
}

Connection::Connection()
{
    conn_ = mysql_init(nullptr);
}
Connection::~Connection()
{
    if(conn_ != nullptr){
        mysql_close(conn_);
    }
}

bool Connection::connect(std::string ip, unsigned short port,std::string user, std::string password, std::string dbname)
{
    MYSQL *p = mysql_real_connect(conn_, ip.c_str(), user.c_str(),password.c_str(), dbname.c_str(), port, nullptr, 0);
    return p != nullptr;
}

bool Connection::update(std::string sql)
{
    if (!conn_) {
        Log("connection is empty");
        return false;
    }
    if(mysql_query(conn_,sql.c_str())){
        //Log("update failed:"+ sql);
        return false;
    }
    return true;
}
MYSQL_RES* Connection::query(std::string sql)
{
    if (!conn_) {
        Log("connection is empty");
        return nullptr;
    }
    if(mysql_query(conn_,sql.c_str())){
        //Log("query failed:" + sql);
        return nullptr;
    }
    return mysql_use_result(conn_);
}


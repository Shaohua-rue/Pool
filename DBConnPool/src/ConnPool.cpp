#include "ConnPool.h"
ConnPool* ConnPool::getConnPool()
{
    static ConnPool pool;
    return &pool;
}
ConnPool::ConnPool()
{
    if(!getConfigFile(userConfig_)){
        return;
    }
    for(int i = 0; i < userConfig_.initSize; i++){
        Connection *p = new Connection();
        p->connect(userConfig_.ip,userConfig_.port,userConfig_.userName,userConfig_.passWd,userConfig_.dbName);
        connQue_.push(p);
        connnectionCnt_++;
    }


    std::thread produce(std::bind(&ConnPool::produceConnectionTask, this));
    produce.detach();

    std::thread scanner(std::bind(&ConnPool::scannerConnectionTask, this));
    scanner.detach();
    
}
std::shared_ptr<Connection> ConnPool::getConnection()
{
    std::unique_lock<std::mutex> locker(queMutex_);
    
    // 等待连接可用或超时
    if(!cv_.wait_for(locker, std::chrono::milliseconds(userConfig_.connectTimeout), [&](){
        return !connQue_.empty();
    })){
        Log("get connection timeout!");
        return nullptr;
    }
    


    // 自定义删除器：当 sp 析构时，将连接放回队列
    std::shared_ptr<Connection> sp(connQue_.front(), [&](Connection* p) {
        // conn 是一个 shared_ptr 的拷贝，用于延长 Connection 对象的生命周期
        // 确保在删除器执行期间，p 不会被释放
        std::unique_lock<std::mutex> lock(queMutex_);
        p->refreshAliveTime();
        connQue_.push(p); // 将原始 shared_ptr 放回队列
    });
    connQue_.pop();
    cv_.notify_one(); // 通知生产者/消费者
    return sp;
}

void ConnPool::produceConnectionTask()
{
    for(;;){
        std::unique_lock<std::mutex> locker(queMutex_);
        cv_.wait(locker, [&](){ 
            return connQue_.size() < userConfig_.maxSize / 2; 
        });

        if(connnectionCnt_ < userConfig_.maxSize){
            Connection *p = new Connection();
            p->connect(userConfig_.ip,userConfig_.port,userConfig_.userName,userConfig_.passWd,userConfig_.dbName);
            connQue_.push(p);
            connnectionCnt_++;
        }
        cv_.notify_all();
    }
}
void ConnPool::scannerConnectionTask()
{
    for(;;){
        std::this_thread::sleep_for(std::chrono::seconds(userConfig_.maxIdleTime));
        std::unique_lock<std::mutex>(queMutex_);
        while(connnectionCnt_ > userConfig_.initSize && !connQue_.empty()){
            auto p = connQue_.front();
            if(std::chrono::duration_cast<std::chrono::seconds>(p->getAliveTime()).count() >= (userConfig_.maxIdleTime)){
                connQue_.pop();
                connnectionCnt_--;
                delete p;
            }else{
                break;
            }
        }   
    }
}

bool ConnPool::getConfigFile(UserConfig& UserConfig)
{
    FILE *fp = fopen("../config/mysql.ini","r");
    if(fp == nullptr)
    {
        Log("mysql.ini open error");
        return false;
    }

    while (!feof(fp))//当文件未到末尾
    {
        ///读取一行数据
        char line[1024] = {0};  
        fgets(line,1024,fp);
        std::string str = line;
        int index = str.find("=", 0);

        if(index == -1) //无效的配置项
        {
            continue;
        }

        int endIndex = str.find("\n", index);
        std::string key = str.substr(0, index);
        std::string value = str.substr(index + 1, endIndex - index - 1);
        
        if(key == "ip")
        {
            UserConfig.ip = value;
        }
        else if(key == "port")
        {
            UserConfig.port = atoi(value.c_str());
        }
        else if(key == "username")
        {
            UserConfig.userName = value;
        }
        else if(key == "password")
        {
            UserConfig.passWd = value;
        }
        else if(key == "dbname")
        {
            UserConfig.dbName = value;
        }
        else if(key == "initSize")
        {
            UserConfig.initSize = atoi(value.c_str());
            
        }
        else if(key == "maxSize")
        {
            UserConfig.maxSize = atoi(value.c_str());
        }
        else if(key == "maxIdleTime")
        {
            UserConfig.maxIdleTime = atoi(value.c_str());
        }
        else if(key == "connectionTimeout")
        {
            UserConfig.connectTimeout = atoi(value.c_str());
        }

    }
    return true;
}
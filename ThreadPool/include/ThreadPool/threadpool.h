#pragma once
#include <thread>
#include <memory>
#include <future>
#include <atomic>
#include <chrono>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <condition_variable>

#include "safequeue.hpp"


enum class ThreadMode{
    MODE_CACHED,
    MODE_FIXED
};

class Thread : public NonCopyable
{
private:
    using Func = std::function<void(int)>;
    Func func_;
    int threadId_;
    static int generateId_;
public:
    Thread(Func func);
    void start();
    int getId() const{return threadId_;}
};

class ThreadPool : public NonCopyable
{
private:
    ThreadMode threadMode_;
    using Task = std::function<void()>;
    std::mutex taskMutex_;
    std::atomic_bool isPoolRunning_;
    std::unordered_map<int, std::unique_ptr<Thread>> threads_;

    SafeQueue<Task> safeTaskQue_;

    int initThreadSize_;
    int maxTaskQueueSize_;
    int maxThreadSize_;
    std::atomic_int idleThreadSize_;

    std::condition_variable notEmptyCond_;
    std::condition_variable notFullCond_;
    std::condition_variable exitCond_;
public:
    ThreadPool();
    ~ThreadPool();

    void start(int initThreadSize);
    void shutdown();

    template<typename Func,typename... Args>
    auto submitTask(Func&& func, Args&&...args) ->std::future<decltype(func(args...))>;

    void setPoolMode(ThreadMode mode);
private:
    void threadFunc(int threadId);
};

template<typename Func,typename... Args>
auto ThreadPool::submitTask(Func&& func, Args&&...args) ->std::future<decltype(func(args...))>
{
    using ReturnType = decltype(func(args...));
    std::function<ReturnType()> func_ = std::bind(std::forward<Func>(func),std::forward<Args>(args)...);
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(func_);
    //auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(func),std::forward<Args>(args)...));

    std::future<ReturnType> future = task->get_future();

    std::unique_lock<std::mutex> lock(taskMutex_);

    if(!notFullCond_.wait_for(lock,std::chrono::seconds(1),[&](){return safeTaskQue_.size() < (size_t)maxTaskQueueSize_;}))
    {
        std::cout<<"task queue is full, submit task faild."<<std::endl;
        auto task = std::make_shared<std::packaged_task<decltype(func(args...))()>>([]()->decltype(func(args...)) {return decltype(func(args...))();});
        (*task)();
        return task->get_future();
    }

    if(threadMode_ == ThreadMode::MODE_CACHED && safeTaskQue_.size() > idleThreadSize_ && threads_.size() < maxThreadSize_){
        std::cout<< "create new thread"<<std::endl;
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc,this,std::placeholders::_1));
        int id = ptr->getId();
        threads_.emplace(id, std::move(ptr));
        threads_[id]->start();
        idleThreadSize_++;
    }
    Task fun = [task](){(*task)();}; 
    safeTaskQue_.push(std::move(fun));

    notEmptyCond_.notify_all();

    return future;
}
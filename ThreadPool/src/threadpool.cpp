#include "ThreadPool/threadpool.h"

int Thread::generateId_ = 0;
const int MAX_TASK_QUEUESIZE = 1024;

const int MAX_THREAD_IDLE_TIME = 10; 
const int MAX_THREAD_SIZE = 1024;

Thread::Thread(Func func):func_(func),threadId_(generateId_++)
{
}

void Thread::start()
{
    std::thread t(func_,threadId_);
    t.detach();
}


ThreadPool::ThreadPool():isPoolRunning_(false),maxTaskQueueSize_(MAX_TASK_QUEUESIZE),
threadMode_(ThreadMode::MODE_FIXED),maxThreadSize_(MAX_THREAD_SIZE)
{

}
ThreadPool::~ThreadPool()
{
    isPoolRunning_ = false;
    std::unique_lock<std::mutex> lock(taskMutex_);
    if(!safeTaskQue_.empty()){
        notEmptyCond_.notify_all();
        exitCond_.wait(lock,[&](){return threads_.empty();});
    }    

}
void ThreadPool::setPoolMode(ThreadMode mode)
{
    threadMode_ = mode;
}
void ThreadPool::start(int initThreadSize = std::thread::hardware_concurrency())
{
    isPoolRunning_ = true;
    initThreadSize_ = initThreadSize;

    for(int id = 0; id < initThreadSize_; id++){
        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc,this,std::placeholders::_1));
        threads_.emplace(id,std::move(ptr));
        threads_[id]->start();
        idleThreadSize_++;
    }
}

void ThreadPool::threadFunc(int threadId)
{
    auto lastTime = std::chrono::high_resolution_clock::now();
    Task task;
    for(;;){
        {
            std::unique_lock<std::mutex> lock(taskMutex_);
            while(safeTaskQue_.empty()){
                if(!isPoolRunning_){
                    threads_.erase(threadId);
                    std::cout<<"threadId: "<< threadId<< " exit"<< std::endl;
                    exitCond_.notify_all();
                    return;
                }
                if(threadMode_ == ThreadMode::MODE_CACHED){
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);
                    if(durationTime.count() >= MAX_THREAD_IDLE_TIME && threads_.size() >initThreadSize_){
                        threads_.erase(threadId);
                        idleThreadSize_--;
                        std::cout<<"cached threadId: "<< threadId<< " exit"<< std::endl;
                        return;
                    }
                }
                else{
                    notEmptyCond_.wait(lock);
                }
                
            }
            idleThreadSize_--;
            safeTaskQue_.pop(task);
            if(safeTaskQue_.empty()){
                notEmptyCond_.notify_all();
            }
            notFullCond_.notify_all();
        }

        if(task != nullptr){
            task();
        }
        idleThreadSize_ ++;
        lastTime = std::chrono::high_resolution_clock::now();
    }
}

void ThreadPool::shutdown()
{
    isPoolRunning_ = false;
    std::unique_lock<std::mutex> lock(taskMutex_);
    if(!safeTaskQue_.empty()){
        notEmptyCond_.notify_all();
        exitCond_.wait(lock,[&](){return threads_.empty();});
    }    
    std::cout<< "thread pool quit"<<std::endl;
}
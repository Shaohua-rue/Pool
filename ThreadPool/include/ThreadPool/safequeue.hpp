#pragma once

#include <queue>
#include <mutex>

class NonCopyable
{
public:
    NonCopyable(const NonCopyable& noncopyable) = delete;
    NonCopyable& operator=(const NonCopyable & noncopyable) = delete; 
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

template <typename T>
class SafeQueue : public NonCopyable
{
private:
    std::queue<T> queue_;
    std::mutex mutex_;
public:
    bool empty() const{
        std::unique_lock<std::mutex>(mutex_);
        return queue_.empty();
    }

    int size() const{
        std::unique_lock<std::mutex>(mutex_);
        return queue_.size();
    }

    void push(T&& t){
        std::unique_lock<std::mutex>(mutex_);
        queue_.emplace(std::move(t));
    }

    bool pop(T& t){
        std::unique_lock<std::mutex>(mutex_);
        if(queue_.empty()){
            return false;
        }
        t = std::move(queue_.front());
        queue_.pop();
        return true;
    }
};

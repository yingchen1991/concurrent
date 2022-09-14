#ifndef _THREAD_SAFE_STACK_LK
#define _THREAD_SAFE_STACK_LK

#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include "inner_exception.h"

template<typename T>
class threadsafe_stack_lk {
private:
    std::stack<T> data;
    mutable std::mutex m;

public:
    threadsafe_stack_lk() { }
    threadsafe_stack_lk(const threadsafe_stack_lk& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    threadsafe_stack_lk& operator=(const threadsafe_stack_lk&) = delete;

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) { return std::shared_ptr<T>(); }
        std::shared_ptr<T> const res(
            std::make_shared<T>(std::move(data.top())));
        data.pop();
        return res;
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            value = nullptr;
            return; 
        }
        value = std::move(data.top());
        data.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};


#endif // _THREAD_SAFE_STACK_LK

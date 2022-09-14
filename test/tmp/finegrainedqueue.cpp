#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

#include "threadsafe_queue_shared_fglk.h"

/*
template<typename T>
class threadsafe_queue_fglk {
private:
    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;

private:    
    node* get_tail() {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head() {
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    std::unique_lock<std::mutex> wait_for_data() {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock, [&]{ return head.get() != get_tail(); });
        return std::move(head_lock);
    }

    std::unique_ptr<node> wait_pop_head() {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head(T& value) {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        value = std::move(*head->data);
        return pop_head();
    }

private:
    std::unique_ptr<node> try_pop_head() {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail()) {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head(T& value) {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail()) {
            return std::unique_ptr<node>();
        }
        value = std::move(*head->data);
        return pop_head();
    }

public:
    std::shared_ptr<T> wait_and_pop() {
        std::unique_ptr<node> const old_head = wait_pop_head();
        return old_head->data;
    }

    void wait_and_pop(T& value) {
        std::unique_ptr<node> const old_head = wait_pop_head(value);
    }

public:
    threadsafe_queue_fglk() : head(new node), tail(head.get()) { }
    threadsafe_queue_fglk(const threadsafe_queue_fglk& other) = delete;
    threadsafe_queue_fglk& operator=(const threadsafe_queue_fglk& other) = delete;

    std::shared_ptr<T> try_pop() {
        std::unique_ptr<node> old_head = try_pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    bool try_pop(T& value) {
        std::unique_ptr<node> const old_head = try_pop_head(value);
        return old_head;
    }
    // std::shared_ptr<T> wait_and_pop();

    // void wait_and_pop(T& value);

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;
            node* const new_tail = p.get();
            tail->next = std::move(p);
            tail = new_tail;
        }
        data_cond.notify_one();
    }

    bool empty() {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head.get() == get_tail());
    }
};

threadsafe_queue_fglk<int> dataqueue;
*/
threadsafe_queue_shared_fglk<int> dataqueue;

void thread_1() {
    for (int i = 0; i < 50; i++) {
        dataqueue.push(i);
    }
}

void thread_2() {
    for (int i = 50; i < 100; i++) {
        dataqueue.push(i);
    }
}

void thread_3() {
    while (!dataqueue.empty()) {
        std::cout << "pop(" << *dataqueue.try_pop() << ") ";
    }
    std::cout << '\n';
}

int main() {
    std::thread t1{thread_1};
    std::thread t2{thread_2};
    std::thread t3{thread_3};

    t1.join();
    t2.join();
    t3.join();

    std::cout << std::endl;

    return 0;
}
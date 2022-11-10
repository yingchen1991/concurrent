#include <atomic>
#include <memory>
#include <thread>
#include <string>
#include <iostream>
#include <chrono>

#include "lockfree/lockfree_queue_hel.h"
#include "lockfreequeueTest.h"
/*
template <typename T>
class lock_free_queue {
private:
    struct node;

    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };

    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;

    struct node_counter {
        unsigned internal_count: 30;
        unsigned external_counters: 2;
    };

    struct node {
        std::atomic<T*> data;
        std::atomic<node_counter> count;
        counted_node_ptr next;

        node() {
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;
            count.store(new_count);

            next.ptr = nullptr;
            next.external_count = 0;
        }

        void release_ref() {
            node_counter old_counter = 
                count.load(std::memory_order_relaxed);
            node_counter new_counter;
            do {
                new_counter = old_counter;
                --new_counter.internal_count;
            } while (!count.compare_exchange_strong(old_counter, new_counter,
                    std::memory_order_acquire, std::memory_order_relaxed));
            if (!new_counter.internal_count && !new_counter.external_counters) {
                delete this;
            }
        }
    };

private:
    static void increase_external_count(
        std::atomic<counted_node_ptr>& counter,
        counted_node_ptr* old_counter) {
        
        counted_node_ptr new_counter;

        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!counter.compare_exchange_strong(old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));
        old_counter->external_count = new_counter.external_count;
    }

    static void free_external_counter(counted_node_ptr &old_node_ptr) {
        node* const ptr = old_node_ptr.ptr;
        int const count_increase = old_node_ptr.external_count - 2;

        node_counter old_counter = 
            ptr->count.load(std::memory_order_relaxed);
        node_counter new_counter;
        do {
            new_counter = old_counter;
            --new_counter.external_counters;
            new_counter.internal_count += count_increase;
        } while (!ptr->count.compare_exchange_strong(old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));
        
        if (!new_counter.internal_count &&
            !new_counter.external_counters) {
            delete ptr;
        }
    }

public:
    void push(T new_value) {
        std::unique_ptr<T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();

        for (;;) {
            increase_external_count(tail, old_tail);

            T* old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong(
                old_data, new_data.get())) {
                old_tail = tail.exchange(new_next);
                free_external_counter(old_tail);
                new_data.release();
                break;
            }
            old_tail.ptr->release_ref();
        }
    }

    std::unique_ptr<T> pop() {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);
        for (;;) {
            increase_external_count(head, old_head);
            node* const ptr = old_head.ptr;
            if (ptr == tail.load().ptr) {
                ptr->release_ref();
                return std::unique_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next)) {
                T* const res = ptr->data.exchange(nullptr);
                free_external_counter(old_head);
                return std::unique_ptr<T>(res);
            }
            ptr->release_ref();
        }
    }
};
*/

/*
template<typename T>
class lock_free_queue {
private:
    struct node;

    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };

    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;

    struct node_counter {
        unsigned internal_count : 30;
        unsigned external_counters : 2;
    };

    struct node {
        std::atomic<T*> data;
        std::atomic<node_counter> count;
        counted_node_ptr next;

        node() {
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;
            count.store(new_count);

            next.ptr = nullptr;
            next.external_count = 0;
        }

        void release_ref() {
            node_counter old_counter = count.load(std::memory_order_relaxed);
            node_counter new_counter;
            do {
                new_counter = old_counter;
                --new_counter.internal_count;
            } while (!count.compare_exchange_strong(old_counter, new_counter,
                    std::memory_order_acquire, std::memory_order_relaxed));
            
            if (!new_counter.internal_count && !new_counter.external_counters) {
                delete this;
            }
        }
    };

private:
    static void increase_external_count(
        std::atomic<counted_node_ptr>& counter, 
        counted_node_ptr& old_counter) {
        counted_node_ptr new_counter;

        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!counter.compare_exchange_strong(old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }

    static void free_external_counter(counted_node_ptr *old_node_ptr) {
        node* const ptr = old_node_ptr->ptr;
        int const count_increase = old_node_ptr->external_count - 2;

        node_counter old_counter = ptr->count.load(std::memory_order_relaxed);
        node_counter new_counter;
        do {
            new_counter = old_counter;
            --new_counter.external_counters;
            new_counter.internal_count += count_increase;
        } while (!ptr->count.compare_exchange_strong(old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));
        if (!new_counter.internal_count && !new_counter.external_counters) {
            delete ptr;
        }
    }

public:
    void push(T new_value) {
        std::unique_ptr<T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();

        for (;;) {
            increase_external_count(tail, old_tail);

            T* old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {
                old_tail.ptr->next = new_next;
                old_tail = tail.exchange(new_next);
                free_external_counter(old_tail);
                new_data.release();
                break;
            }
            old_tail.ptr->release_ref();
        }
    }

    std::unique_ptr<T> pop() {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);
        for (;;) {
            increase_external_count(head, old_head);
            node* const ptr = old_head.ptr;
            if (ptr == tail.load().ptr) {
                ptr->release_ref();
                return std::unique_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next)) {
                T* const res = ptr->data.exchange(nullptr);
                free_external_counter(old_head);
                return std::unique_ptr<T>(res);
            }
            ptr->release_ref();
        }
    }
};
*/

/*
template<typename T>
class lock_free_queue {
private:
    struct node;

    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };

    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;

    struct node_counter {
        unsigned internal_count : 30;
        unsigned external_counters : 2;
    };

    struct node {
        std::atomic<T*> data;
        std::atomic<node_counter> count;
        std::atomic<counted_node_ptr> next;

        node() {
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;
            count.store(new_count);

            // next.load().ptr = nullptr;
            // next.load().external_count = 0;
            next.store({0});
        }

        void release_ref() {
            node_counter old_counter = count.load(std::memory_order_relaxed);
            node_counter new_counter;

            do {
                new_counter = old_counter;
                --new_counter.internal_count;
            } while (!count.compare_exchange_strong(old_counter, new_counter,
                    std::memory_order_acquire, std::memory_order_relaxed));
            
            if (!new_counter.internal_count && !new_counter.external_counters) {
                delete this;
            }
        }
    };

private:
    static void increase_external_count(std::atomic<counted_node_ptr>& counter, 
        counted_node_ptr& old_counter) {
        counted_node_ptr new_counter;

        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!counter.compare_exchange_strong(old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));

        old_counter.external_count = new_counter.external_count;
    }

    static void free_external_counter(counted_node_ptr& old_node_ptr) {
        node* const ptr = old_node_ptr.ptr;
        int const count_increase = old_node_ptr.external_count - 2;

        node_counter old_counter = ptr->count.load(std::memory_order_relaxed);
        node_counter new_counter;
        do {
            new_counter = old_counter;
            --new_counter.external_counters;
            new_counter.internal_count += count_increase;
        } while (!ptr->count.compare_exchange_strong(old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));
        
        if (!new_counter.internal_count && !new_counter.external_counters) {
            delete ptr;
        }
    }

    void set_new_tail(counted_node_ptr &old_tail, counted_node_ptr const &new_tail) {
        node* const current_tail_ptr = old_tail.ptr;
        while (!tail.compare_exchange_weak(old_tail, new_tail) && 
            old_tail.ptr == current_tail_ptr);
        if (old_tail.ptr == current_tail_ptr) {
            free_external_counter(old_tail);
        } else {
            current_tail_ptr->release_ref();
        }
    }

public:
    void push(T new_value) {
        std::unique_ptr<T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();

        for (;;) {
            increase_external_count(tail, old_tail);

            T* old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {
                counted_node_ptr old_next = {0};
                if (!old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {
                    delete new_next.ptr;
                    new_next = old_next;
                }
                set_new_tail(old_tail, new_next);
                new_data.release();
                break;
            } else {
                counted_node_ptr old_next = {0};
                if (old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {
                    old_next = new_next;
                    new_next.ptr = new node;
                }
                set_new_tail(old_tail, old_next);
            }
        }
    }

    std::unique_ptr<T> pop() {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);

        for (;;) {
            increase_external_count(head, old_head);
            node* const ptr = old_head.ptr;
            if (ptr == tail.load().ptr) {
                return std::unique_ptr<T>();
            }
            counted_node_ptr next = ptr->next.load();
            if (head.compare_exchange_strong(old_head, next)) {
                T* const res = ptr->data.exchange(nullptr);
                free_external_counter(old_head);
                return std::unique_ptr<T>(res);
            }
            ptr->release_ref();
        }
    }
};

*/

lock_free_queue_hel<std::string> data_queue;
// lock_free_queue<std::string> data_queue;


void thread_1() {
    std::cout << "in thread 1 ...\n";
    for (int i = 0; i < 100; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        data_queue.push(std::string("thread_1_") + std::to_string(i));
        std::cout << "push(" << "thread_1_" + std::to_string(i) << ")" << std::endl;
    }
}

void thread_2() {
    std::cout << "in thread 2 ...\n";
    for (int i = 0; i < 100; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        data_queue.push(std::string("thread_2_") + std::to_string(i));
    }
}

void thread_3() {
    std::cout << "in thread 3 ...\n";
    for (int i = 0; i < 50; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        auto data = data_queue.pop();
        if (data != nullptr) {
            std::cout << "pop(" << *data << ") \n";
        }
    }
    std::cout << std::endl;
}

void thread_4() {
    std::cout << "in thread 4 ...\n";
    for (int i = 0; i < 50; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        // std::cout << *data_queue.pop() << " ";
        auto data = data_queue.pop();
        if (data != nullptr) {
            std::cout << "pop(" << *data << ") \n";
        }
    }
    std::cout << std::endl;
}

int main () {
    std::cout << "test lock free queue ... " << '\n';

    std::thread t1{thread_1};
    std::thread t2{thread_2};
    std::thread t3{thread_3};
    std::thread t4{thread_4};

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::atomic<std::string*> aStr{nullptr};
    std::string* old_str = nullptr;
    std::string* new_str = new std::string("hello");
    if (aStr.compare_exchange_strong(old_str, new_str)) {
        std::cout << "aStr : " << *aStr.load() << '\n';
    } else {
        std::cout << "failed\n";
    }

    std::cout << "the reset data are: \n";
    std::shared_ptr<std::string> strPtr;
    while ((strPtr = data_queue.pop()) != std::shared_ptr<std::string>()) {
        std::cout << *strPtr << " ";
    }
    std::cout << std::endl;

    return 0;
}

// node 初始化时必须赋初值，否则会有一个随机初始值
// pop注意判空
// 构造函数初始化
// 释放
// 

#include <memory>
#include <atomic>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

#include "lockfree/lockfree_stack_ref.h"

/*
template<typename T>
class lock_free_stack {
private:
    struct node;

    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };

    struct node {
        std::shared_ptr<T> data;
        std::atomic<int> internal_count;
        counted_node_ptr next;

        node(T const& data_): 
            data(std::make_shared<T>(data_)), 
            internal_count(0) { }
    };

    std::atomic<counted_node_ptr> head;

private:
    void increase_head_count(counted_node_ptr& old_counter) {
        counted_node_ptr new_counter;

        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!head.compare_exchange_strong(old_counter, new_counter));

        old_counter.external_count = new_counter.external_count;
    }

public:
    ~lock_free_stack() {
        while (pop());
    }

    void push(T const& data) {
        counted_node_ptr new_node;
        new_node.ptr = new node(data);
        new_node.external_count = 1;
        new_node.ptr->next = head.load();
        while (!head.compare_exchange_weak(new_node.ptr->next, new_node));
    }

    std::shared_ptr<T> pop() {
        counted_node_ptr old_head = head.load();
        for (;;) {
            increase_head_count(old_head);
            node* const ptr = old_head.ptr;
            if (!ptr) {
                return std::shared_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next)) {
                std::shared_ptr<T> res;
                res.swap(ptr->data);

                int const count_increase = old_head.external_count - 2;

                if (ptr->internal_count.fetch_add(count_increase) == -count_increase) {
                    delete ptr;
                }
                return res;
            } else if (ptr->internal_count.fetch_sub(1) == 1) {
                delete ptr;
            }
        }
    }
};

lock_free_stack<std::string>  data_stack;
*/

lock_free_stack_ref<std::string> data_stack;

void thread_1() {
    for(int i = 0; i < 100; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        data_stack.push(std::string("thread_1_") + std::to_string(i));
    }
}

void thread_2() {
    for (int i = 0; i < 100; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        data_stack.push(std::string("thread_2_") + std::to_string(i));
    }
}

void thread_3() {
    for (int i = 0; i < 50; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        std::cout << *data_stack.pop() << " ";
    }
}

void thread_4() { 
    for (int i = 0; i < 50; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        std::cout << *data_stack.pop() << " ";
    }
}

int main() {
    std::cout << "Test lock free stack ..." << '\n';

    std::thread t1{thread_1};
    std::thread t2{thread_2};
    std::thread t3{thread_3};
    std::thread t4{thread_4};

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "the rest data are: ";
    std::shared_ptr<std::string> strPtr;
    while ((strPtr = data_stack.pop()) != std::shared_ptr<std::string>()) {
        std::cout << *strPtr << " ";
    }
    std::cout << std::endl;

    return 0;
}
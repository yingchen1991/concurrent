#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>

template <typename T>
class lock_free_stack {
private:
    struct node {
        std::shared_ptr<T> data;
        node* next;

        node(T const& data_) : data(std::make_shared<T>(data_)) { }
    };
    std::atomic<node*> head;

private:
    std::atomic<unsigned> threads_in_pop;
    std::atomic<node*> to_be_deleted;

    static void delete_nodes(node* nodes) {
        while (nodes)
        {
            node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    void try_reclaim(node* old_head) {
        if (threads_in_pop == 1) {
            node* nodes_to_delete = to_be_deleted.exchange(nullptr);
            if (!--threads_in_pop) {
                delete_nodes(nodes_to_delete);
            } else if (nodes_to_delete) {
                chain_pending_nodes(nodes_to_delete);
            }
            delete old_head;
        } else {
            chain_pending_node(old_head);
            --threads_in_pop;
        }
    }

    void chain_pending_nodes(node* nodes) {
        node* last = nodes;
        while (node* const next = last->next) {
            last = next;
        }
        chain_pending_nodes(nodes, last);
    }

    void chain_pending_nodes(node* first, node* last) {
        last->next = to_be_deleted;
        while (!to_be_deleted.compare_exchange_weak(last->next, first));
    }

    void chain_pending_node(node* n) {
        chain_pending_nodes(n, n);
    }

public:
    void push(T const& data) {
        node* const new_node = new node(data);
        new_node->next = head.load();
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }

    std::shared_ptr<T> pop() {
        ++threads_in_pop;
        node* old_head = head.load();
        while (old_head && head.compare_exchange_weak(old_head, old_head->next));
        std::shared_ptr<T> res;
        if (old_head) {
            res.swap(old_head->data);
        }
        try_reclaim(old_head);
        return res;
    }
};

lock_free_stack<std::string> data_stack;

void thread_1() {
    for (int i = 0; i < 100; i++) {
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
    std::cout << std::endl;
}

void thread_4() {
    for (int i = 0; i < 50; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        std::cout << *data_stack.pop() << " ";
    }
    std::cout << '\n';
}


int main() {
    std::cout << "Test lock free stack ... " << '\n';

    std::thread t1{thread_1};
    std::thread t2{thread_2};
    std::thread t3{thread_3};
    std::thread t4{thread_4};

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "the reset data are: ";
    std::shared_ptr<std::string> strPtr;
    while ((strPtr = data_stack.pop()) != std::shared_ptr<std::string>()) {
        std::cout << *strPtr << " ";
    }
    std::cout << std::endl;

    std::shared_ptr<int> data;

    std::cout << "test ok!!! " << std::atomic_is_lock_free(&data) << std::endl;

    return 0;
}
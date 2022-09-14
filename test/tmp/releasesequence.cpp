#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

std::vector<int> queue_data;
std::atomic<int> count;

void populate_queue() {
    unsigned const number_of_items = 20;
    queue_data.clear();
    for (unsigned i = 0; i < number_of_items; ++i) {
        queue_data.push_back(i);
    }

    count.store(number_of_items, std::memory_order_release);
}

void wait_for_more_items() {
    std::cout << "wait for more ..." << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void process(int data) {
    // std::cout << "*********" << '\n'
    //           << "* " << data << '\t' << '*' << '\n'
    //           << "*********" << '\n';
    std::cout << data << '\n';
}

void consume_queue_items() {
    while (true) {
        int item_index;
        if ((item_index = count.fetch_sub(1, std::memory_order_acquire)) <= 0) {
            // wait_for_more_items();
            // continue;
            break;
        }
        process(queue_data[item_index-1]);
    }
}

int main() {
    std::thread a{populate_queue};
    std::thread b{consume_queue_items};
    std::thread c{consume_queue_items};
    a.join();
    b.join();
    c.join();
}
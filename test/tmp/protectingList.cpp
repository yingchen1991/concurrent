#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), value_to_find)
        != some_list.end();
}

int main() {
    std::cout << "start testing ..." << '\n';
    std::thread task1([](){
        std::cout << "task1 begin" << '\n';
        for (int i = 0; i < 10; i++) {
            add_to_list(i);
        }
        std::cout << "task1 finished." << '\n';
    });

    std::thread task2([](){
        std::cout << "task2 begin" << '\n';
        for (int i = 10; i < 20; i++) {
            add_to_list(i);
        }
        std::cout << "task2 finished." << '\n';
    });

    std::thread task3([](){
        std::cout << "task3 begin" << '\n';
        for (int i = 0; i < 20; i++) {
            if (!list_contains(i)) {
                std::cout << "list not contains: " << i << '\n';
            }
        }
    });

    task1.join();
    task2.join();
    task3.join();

    std::cout << "main task continue..." << '\n';
    for (int i = 0; i < 20; i++) {
        if (!list_contains(i)) {
            std::cout << "list still not contains: " << i << '\n';
        }
    }
    bool b = true;
    std::cout << "main task finished!" << std::to_string(true) << '\n';

    return 0;
}
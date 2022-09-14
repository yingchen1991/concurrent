#include <thread>
#include <atomic>
#include <cassert>
#include <iostream>

std::atomic<int> data[5];
std::atomic<int> sync0{0};

void thread_1() {
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    sync0.store(true, std::memory_order_release);
}

void thread_2() {
    int excpected = 1;
    while (!sync0.compare_exchange_strong(excpected, 2, std::memory_order_acq_rel)) {
        excpected = 1;
    }
}

void thread_3() {
    while (sync0.load(std::memory_order_acquire) < 2);
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
}

int main() {
    std::thread t1{thread_1};
    std::thread t2{thread_2};
    std::thread t3{thread_3};

    t1.join();
    t2.join();
    t3.join();

    std::cout << "------> test finished!" << '\n';
    return 0;
}
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> cnt = {0};

void f() {
    for (int i = 0; i < 1000; i++) {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    std::vector<std::thread> v;
    for (int i = 0; i < 10; i++) {
        v.emplace_back(f);
    }

    for (auto& t : v) {
        t.join();
    }

    std::cout << "Final counter value is """": " << cnt << '\n';
}
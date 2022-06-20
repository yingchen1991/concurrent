#include "utils/thread_guard.h"

#include <iostream>

struct func
{
    int state_;
public:
    func(int state) : state_(state) { }
    void operator()() {
        std::cout << "func processing :" << state_ << '\n';
    }
};

void do_something_in_current_thread() {
    std::cout << "do something in current thread ..." << '\n';
}

void f() {
    int some_local_sate=0;
    func my_func(some_local_sate);
    std::thread t(my_func);
    thread_guard g(t);

    do_something_in_current_thread();
}

int main() {
    std::cout << "start test ..." << '\n';
    f();

    std::cout << "finish test ..." << '\n';
    return 0;
}

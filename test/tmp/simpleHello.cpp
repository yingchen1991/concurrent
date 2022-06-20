#include <iostream>
#include <thread>

void hello()
{
    std::cout << "Hello Concurrency !!!\n";
    std::cout << "hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
}

int main()
{
    std::thread t(hello);
    t.join();
}
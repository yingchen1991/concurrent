#include <thread>
#include <stdexcept>

class scoped_thread {
public:
    explicit scoped_thread(std::thread t) :
        t_(std::move(t)) 
    {
        if (!t.joinable()) {
            throw std::logic_error("No thread");
        }
    }
    ~scoped_thread() {
        t_.join();
    }
    
    scoped_thread(scoped_thread const&)=delete;
    scoped_thread& operator=(scoped_thread const&)=delete;

private:
    std::thread t_;
};
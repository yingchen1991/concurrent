#include <mutex>

class some_big_object{

};

void swap(some_big_object& lhs, some_big_object& rhs);

class X {
private:
    some_big_object some_detail;
    std::mutex m;
public:
    X(some_big_object const& sd): some_detail(sd) {}

    friend void swap(X& lhs, X& rhs) {
        if (&lhs == &rhs) {
            return;
        }
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        std::lock(lock_a, lock_b);
        swap(lhs.some_detail, rhs.some_detail);
    }
};

std::unique_lock<std::mutex> get_lock() {
    extern std::mutex some_mutex;
    std::unique_lock<std::mutex> lk(some_mutex);
    // prepare_data();
    return lk;
}
void process_data() {
    std::unique_lock<std::mutex> lk(get_lock());
    // do_something();
}

void get_and_process_data() {
    std::unique_lock<std::mutex> my_lock(the_mutex);
    some_class data_to_process = get_next_data_chunk();
    mu_lock.unlock();
    result_type result = process(data_to_process);
    my_lock.lock();
    write_result(data_to_process, result);
}

class Y {
private:
    int some_detail;
    mutable std::mutex m;

    int get_detail() const {
        std::lock_guard<std::mutex> lock_a(m);
        return some_detail;
    }
public:
    Y(int sd): some_detail(sd) { }

    friend bool operator==(Y const& lhs, Y const& rhs) {
        if (&lhs == &rhs) {
            return true;
        }
        int const lhs_value = lhs.get_detail();
        int const rhs_value = rhs.get_detail();
        return lhs_value == rhs_value;
    }
};

std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag;

void init_resource() {
    resource_ptr.reset(new some_resource);
}
void foo() {
    std::call_once(resource_flag, init_resource);
    resource_ptr->do_something();
}

class xtemp {
private:
    connection_info connection_details;
    connection_handle connection;
    std::once_flag connection_init_flag;

    void open_connection() {
        connection = connection_manager.open(connection_details);
    }
public:
    xtemp(connection_info const& connection_details_):
        connection_details(connection_details_)
    {}
    void send_data(data_packet const& data) {
        std::call_once(connection_init_flag, &xtemp::open_connection, this);
        connection.send_data(data);
    }
    data_pacet receive_data() {
        std::call_once(connection_init_flag, &xtemp::open_connection, this);
        return connection.receive_data();
    }
};

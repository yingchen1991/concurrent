// #include "boost/thread.hpp"
#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/locks.hpp"
#include <list>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <functional>
#include <mutex>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

#include "threadsafe_lookup_table_hash.h"

/*
template<typename Key, typename Value, typename Hash=std::hash<Key>>
class threadsafe_lookup_table {
private:
    class bucket_type {
    public:
        typedef std::pair<Key, Value> bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::iterator bucket_iterator;

        bucket_data data;
        mutable boost::shared_mutex mutex;

        bucket_iterator find_entry_for(Key const& key) {
            return std::find_if(data.begin(), data.end(),
                                [&](bucket_value const& item) 
                                { return item.first == key; });
        }

    public:
        Value value_for(Key const& key, Value const& default_value) {
            boost::shared_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry = find_entry_for(key);
            return (found_entry == data.end()) ? 
                default_value : found_entry->second;
        }

        void add_or_update_mapping(Key const& key, Value const& value) {
            std::unique_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry = find_entry_for(key);
            if (found_entry == data.end()) {
                data.push_back(bucket_value(key, value));
            } else {
                found_entry->second = value;
            }
        }

        void remove_mapping(Key const& key) {
            std::unique_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry = find_entry_for(key);
            if (found_entry != data.end()) {
                data.erase(found_entry);
            }
        }
    };

    std::vector<std::unique_ptr<bucket_type>> buckets;
    Hash hasher;

    bucket_type& get_bucket(Key const& key) const {
        std::size_t const bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef Hash hash_type;

    threadsafe_lookup_table(
        unsigned num_buckets = 19, Hash const& hasher_ = Hash()) :
        buckets(num_buckets), hasher(hasher_)
    {
        for (unsigned i = 0; i < num_buckets; ++i) {
            buckets[i].reset(new bucket_type);
        }
    }

    threadsafe_lookup_table(threadsafe_lookup_table const& other) = delete;
    threadsafe_lookup_table& operator=(threadsafe_lookup_table const& other) = delete;

    Value value_fo(Key const& key,
                   Value const& default_value = Value()) const
    {
        return get_bucket(key).value_for(key, default_value);
    }

    void add_or_update_mapping(Key const& key, Value const& value) {
        get_bucket(key).add_or_update_mapping(key, value);
    }

    void remove_mapping(Key const& key) {
        get_bucket(key).remove_mapping(key);
    }

    std::map<Key, Value> get_map() const {
        std::vector<std::unique_lock<boost::shared_mutex>> locks;
        for (unsigned i = 0; i < buckets.size(); ++i) {
            locks.push_back(
                std::unique_lock<boost::shared_mutex>(buckets[i]->mutex));
        }
        std::map<Key, Value> res;
        for (unsigned i = 0; i < buckets.size(); ++i) {
            typedef typename bucket_type::bucket_iterator iterator;
            for (iterator it = (iterator)(buckets[i]->data.begin()); it != buckets[i]->data.end(); ++it)
            {
                res.insert(*it);
            }
        }
        return res;
    }
};

threadsafe_lookup_table<std::string, int, std::hash<std::string>> datatable;
*/
threadsafe_lookup_table_hash<std::string, int, std::hash<std::string>> datatable;

void thread_1() {
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 100; j++) {
            datatable.add_or_update_mapping("value_"+std::to_string(i)+"_"+std::to_string(j), i+j);
        }
    }
}

void thread_2() {
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 100; j++) {
            std::cout << "get: " << datatable.value_fo("value_"+std::to_string(i)+"_"+std::to_string(j), 999999);
        }
    }
    std::cout << '\n';
}

void thread_3() {
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 100; j++) {
            datatable.remove_mapping("value_"+std::to_string(i)+"_"+std::to_string(j));
            std::cout << "delete(" << "value_"+std::to_string(i)+"_"+std::to_string(j) << ") ";
        }
    }
    std::cout << '\n';
}

void thread_4() { 
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "get map: ";
    std::map<std::string, int> map = datatable.get_map();
    for (std::map<std::string, int>::iterator it = map.begin(); it != map.end(); it++) {
        std::cout << "(" << it->first << ", " << it->second << ") ";
    }

    std::cout << '\n';
}

int main() {
    std::thread t1{thread_1};
    std::thread t2{thread_2};
    std::thread t3{thread_3};
    std::thread t4{thread_4};

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
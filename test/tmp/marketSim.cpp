// Simulating a supermarket with multiple checkouts 
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <random>
#include "Customer.h"
#include "Checkout.h"

#include <chrono>

using std::string;
using distribution = std::uniform_int_distribution<>;

// Output histogram of service times
void histogram(const std::vector<int>& v, int min) {
    string bar(60, '*');
    for (size_t i {}; i < v.size(); ++i) {
        std::cout << std::setw(3) << i + min << " "
        << std::setw(4) << v[i] << " " 
        << bar.substr(0, v[i])
        << (v[i] > static_cast<int>(bar.size()) ? "..." : "")
        << std::endl;
    }
}

int main() {
    std::random_device random_n;

    // Setup minimum & maximum checkout periods - times in minutes
    int service_t_min{2}, service_t_max{15};
    distribution service_t_d{service_t_min, service_t_max};

    // Setup minimum & maximum number of customers at store opening 
    int min_customers{15}, max_customers{20};
    distribution n_1st_customers_d{min_customers, max_customers};

    // Setup minimum & maximum intervals between customer arrivals
    int min_arr_interval{1}, max_arr_interval{5};
    distribution arrival_interval_d{min_arr_interval, max_arr_interval};

    size_t n_checkouts{};
    std::cout << "Enter the number of checkouts in the supermarket: ";
    std::cin >> n_checkouts;
    if (!n_checkouts) {
        std::cout << "Number of checkouts must be greater than 0. Setting to 1." << 
        std::endl;
        n_checkouts = 1;
    }

    std::vector<Checkout> checkouts{n_checkouts};
    std::vector<int> service_time(service_t_max - service_t_min + 1);

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Add customers waiting when store opens 
    int count{n_1st_customers_d(random_n)};
    std::cout << "Customers waiting at store opening: " << count << std::endl;
    int added{};
    int service_t{};
    while (added++ < count) {
        service_t = service_t_d(random_n);
        std::min_element(std::begin(checkouts), std::end(checkouts))->add(Customer(service_t));
        ++service_time[service_t - service_t_min];
    }

    size_t time{};
    const size_t total_time{600};
    size_t longest_q{};

    // Period until next customer arrives
    int new_cust_interval{arrival_interval_d(random_n)};

    // Run store simulation for period of total_time minutes
    while (time < total_time) {
        ++time;

        // new customer arrives when arrival interval is zero
        if (--new_cust_interval == 0) {
            service_t = service_t_d(random_n);
            std::min_element(std::begin(checkouts), std::end(checkouts))->add(Customer(service_t));
            ++service_time[service_t - service_t_min];  //Record service time

            // Update record of the longest queue occurring
            for (auto& checkout : checkouts) {
                longest_q = std::max(longest_q, checkout.qlength());
            }

            new_cust_interval = arrival_interval_d(random_n);
        }

        // Update the time in the checkouts - serving the 1st customer in each queue
        for (auto& checkout : checkouts) {
            checkout.time_increment();
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    std::cout << "Maximum queue length = " << longest_q << std::endl;
    std::cout << "\nHistogram of service times:\n";
    histogram(service_time, service_t_min);

    std::cout << "\nTotal number of customers today: " 
            << std::accumulate(std::begin(service_time), std::end(service_time), 0)
            << std::endl;
    
    std::chrono::duration<double, std::milli> fp_ms = end - start;
    std::cout << "The total time of simulation takes : " << fp_ms.count() << " ms" << '\n';
}
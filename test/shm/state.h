#ifndef _STATE_H_
#define _STATE_H_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

class State {
public:
    explicit State(const uint64_t& ceiling_msg_size) : ceiling_msg_size_(ceiling_msg_size) { }
    virtual ~State() { }

    void DecreaseReferenceCounts() {
        uint32_t current_reference_count = reference_count_.load();
        do {
            if (current_reference_count == 0) {
                return;
            }
        } while (!reference_count_.compare_exchange_strong(
            current_reference_count, current_reference_count - 1));
    }

    void IncreaseReferenceCounts() { reference_count_.fetch_add(1); }

    uint32_t FetchAddSeq(uint32_t diff) { return seq_.fetch_add(diff); }
    uint32_t seq() { return seq_.load(); }

    void set_need_remap(bool need) { need_remap_.store(need); }
    bool need_remap() { return need_remap_; }

    uint64_t ceiling_msg_size() { return ceiling_msg_size_.load(); }
    uint32_t reference_counts() { return reference_count_.load(); }

private:
    std::atomic<bool> need_remap_ = {false};
    std::atomic<uint32_t> seq_ = {0};
    std::atomic<uint32_t> reference_count_ = {0};
    std::atomic<uint64_t> ceiling_msg_size_;
};


#endif // _STATE_H_
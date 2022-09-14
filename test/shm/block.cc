#include "block.h"
#include <iostream>

const int32_t Block::kRWLockFree = 0;
const int32_t Block::kWriteExclusive = -1;
const int32_t Block::kMaxTryLockTimes = 5;

Block::Block() : msg_size_(0), msg_info_size_(0) { }

Block::~Block() { }

bool Block::TryLockForWrite() {
    int32_t rw_lock_free = kRWLockFree;
    if (!lock_num_.compare_exchange_weak(rw_lock_free, kWriteExclusive,
                                         std::memory_order_acq_rel,
                                         std::memory_order_relaxed))
    {
        std::cout << "lock num: " << lock_num_.load();
        return false;
    }
    return true;
}

bool Block::TryLockForRead() { 
    int32_t lock_num = lock_num_.load();
    if (lock_num < kRWLockFree) {
        std::cout << "block is being written.";
        return false;
    }

    int32_t try_times = 0;
    while (!lock_num_.compare_exchange_weak(lock_num, lock_num + 1,
                                            std::memory_order_acq_rel,
                                            std::memory_order_relaxed)) 
    {
        ++try_times;
        if (try_times == kMaxTryLockTimes) {
            std::cout << "fail to add read lock num, curr num: " << lock_num;
            return false;
        }

        lock_num = lock_num_.load();
        if (lock_num < kRWLockFree) {
            std::cout << "block is being written.";
            return false;
        }
    }

    return true;
}

void Block::ReleaseWriteLock() { lock_num_.fetch_add(1); }

void Block::ReleaseReadLock() { lock_num_.fetch_sub(1); }
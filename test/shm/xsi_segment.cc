#include "xsi_segment.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <iostream>

XsiSegment::XsiSegment(uint64_t channel_id) : Segment(channel_id) {
    key_ = static_cast<key_t>(channel_id);
}

XsiSegment::~XsiSegment() { Destroy(); }

bool XsiSegment::OpenOrCreate() {
    if (init_) {
        return true;
    }

    // create managed_shm_
    int retry = 0;
    int shmid = 0;
    while (retry < 2) {
        shmid = shmget(key_, conf_.managed_shm_size(), 0644 | IPC_CREATE | IPC_EXCL);
        if (shmid != -1) {
            break;
        }

        if (EINVAL == error) {
            std::cout << "need larger space, recreate.";
            Reset();
            Remove();
            ++retry;
        } else if (EEXIST == error) {
            std::cout << "shm already exist, open only.";
            return OpenOnly();
        } else {
            break;
        }
    }

    if (shmid == -1) {
        std::cout << "create shm failed, error code: " << strerror(errno);
        return false;
    }

    // attach managed_shm_
    managed_shm_ = shmat(shmid, nullptr, 0);
    if (managed_shm_ == reinterpret_cast<void*>(-1)) {
        std::cout << "attach shm failed, error: " << strerror(errno);
        shmctl(shmid, IPC_RMID, 0);
        return false;
    }

    // create field state_
    state_ = new (managed_shm_) State(conf_.ceiling_msg_size());
    if (state_ == nullptr) { 
        std::cout << "create state failed.";
        shmdt(managed_shm_);
        managed_shm_ = nullptr;
        shmctl(shmid, IPC_RMID, 0);
        return false;
    }

    conf_.Update(state_->ceiling_msg_size());

    // create field blocks_
    blocks_ = new (static_cast<char*>(managed_shm_) + sizeof(State)) Block[conf_.block_num()];
    if (blocks_ == nullptr) {
        std::cout << "create blocks failed. ";
        state_->~State();
        state_ = nullptr;
        shmdt(shmid, IPC_RMID, 0);
        return false;
    }

    // create block buf
    uint32_t i = 0; 
    for (; i < conf_.block_num(); ++i) {
        uint8_t* addr = 
            new (static_cast<char*>(managed_shm_) + sizeof(State) + 
                 conf_.block_num() * sizeof(Block) + i * conf_.block_buf_size())
                 uint8_t[conf_.block_buf_size()];
        std::lock_guard<std::mutex> _g(block_buf_lock_);
        block_buf_addrs_[i] = addr;
    }

    if (i != conf_.block_num()) {
        std::cout << "create block buf failed.";
        state_->~State();
        state_ = nullptr;
        blocks_ = nullptr;
        {
            std::lock_guard<std::mutex> _g(block_buf_lock_);
            block_buf_addrs_.clear();
        }
        shmdt(managed_shm_);
        managed_shm_ = nullptr;
        shmctl(shmid, IPC_RMID, 0);
        return false;
    }

    state_->IncreaseReferenceCounts();
    init = true;
    std::cout << "open or create true.";
    return true;
}
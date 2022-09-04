//
// Created by Cu1 on 2022/8/23.
//

#ifndef ZEPHYR_LOKI_ALLOCATOR_H
#define ZEPHYR_LOKI_ALLOCATOR_H

#include <new>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace zephyr
{

struct chunk {

public:
    unsigned char first_available_block_;
    size_t block_available_;
    unsigned char* p_data_;

public:

    void reset(size_t block_size_, unsigned char blocks_) {
        block_available_ = blocks_;
        first_available_block_ = 0;
        unsigned char i = 0;
        unsigned char* p = p_data_;
        for (; i != blocks_; p += block_size_) {
            *p = ++i;
            std::cout << "p = " << (void*)p << " i = " << (int)i << std::endl;
        }
        std::cout << "reset(size_t block_size_, size_t blocks_)::p_data_ = " << (void*)p_data_ << std::endl;
        std::cout << "reset(size_t block_size_, size_t blocks_)::*p_data_ = " << (int)*p_data_ << std::endl;
    }

    void init(size_t block_size_, unsigned char blocks_) {
        //DEBUG
        std::cout << "init(size_t block_size_, size_t blocks_)::block_size_ = " << block_size_ << " blocks_ = " << (unsigned int)blocks_ << std::endl;
        try {
            p_data_ = new unsigned char[block_size_ * blocks_];
        }
        catch (...) {
            std::cout << "memory not enough" << std::endl;
            release();
        }
        // DEBUG
        std::cout << "init(size_t block_size_, size_t blocks_):: chunk: init = " << block_size_ * blocks_ << std::endl;
        std::cout << "init(size_t block_size_, size_t blocks_):: p_data_ = " << (void*)p_data_ << std::endl;
        reset(block_size_, blocks_);
    }

    void release() { delete[] p_data_; }


public:

    chunk() = default;

    chunk(size_t block_size_,  unsigned char blocks_) {
        init(block_size_, blocks_);
    }

    void* allocate(size_t block_size_) {
        if (!block_available_) return nullptr;

        // DEBUG
        std::cout << "allocate(size_t block_size_)::this = " << this << std::endl;

        // DEBUG
        std::cout << "allocate(size_t block_size_)::p_data_ = " << (void*)p_data_ << std::endl;
        std::cout << "allocate(size_t block_size_)::*p_data_ = " << (unsigned int)*p_data_ << std::endl;

        // DEBUG
        std::cout << "allocate(size_t block_size_)::block_available_ = " << block_available_ << std::endl;
        std::cout << "allocate(size_t block_size_)::first_available_block_ = " << (unsigned int)first_available_block_ << std::endl;

        unsigned char* p_result_ =
                p_data_ + (block_size_ * first_available_block_);

        // DEBUG
        std::cout << "allocate(size_t block_size_)::p_data_ = " << (void*)p_data_ << std::endl;
        std::cout << "allocate(size_t block_size_)::p_result_ = " << (void*)p_result_ << std::endl;
        std::cout << "allocate(size_t block_size_)::*p_data_ = " << (unsigned int)*p_data_ << std::endl;
        std::cout << "allocate(size_t block_size_)::*p_result_ = " << (unsigned int)*p_result_ << std::endl;


        first_available_block_ = *p_result_;
        --block_available_;

        // DEBUG
        std::cout << "allocate(size_t block_size_)::block_available_ = " << block_available_ << std::endl;
        std::cout << "allocate(size_t block_size_)::first_available_block_ = " << (unsigned int)first_available_block_ << std::endl;

        return p_result_;
    }

    void deallocate(void* p, size_t block_size_) {
        unsigned char* to_release_ = static_cast<unsigned char*>(p);
        *to_release_ = first_available_block_;
        first_available_block_ =
                static_cast<unsigned char>((to_release_ - p_data_) / block_size_);
        ++block_available_;
    }

    ~chunk() { release(); }

};

class fixed_allocator {

private:
    chunk* alloc_chunk_;
    chunk* dealloc_chunk_;
    std::vector<chunk>  chunks_;
    size_t block_size_;
    unsigned char num_blocks_;

public:
    fixed_allocator(size_t block_size, unsigned char num_blocks)
        : alloc_chunk_(nullptr),
          dealloc_chunk_(nullptr),
          block_size_(block_size),
          num_blocks_(num_blocks)
    {}

    void* allocate() {
        if (alloc_chunk_ == nullptr || alloc_chunk_->block_available_ == 0) {
            auto i = chunks_.begin();
            for (;; ++i) {
                if (i == chunks_.end()) {
                    chunks_.push_back(chunk(block_size_, num_blocks_));
                    alloc_chunk_ = &chunks_.back();
                    dealloc_chunk_ = &chunks_.front();

                    //DEBUG
                    std::cout << "allocate()::alloc_chunk_ = " << alloc_chunk_ << " " << dealloc_chunk_ << std::endl;

                    break;
                }
                if (i->block_available_ > 0) {
                    alloc_chunk_ = &*i;
                    break;
                }
            }
        }
        return alloc_chunk_->allocate(block_size_);
    }

    void deallocate(void* p) {
        dealloc_chunk_ = deallocate_chunk_find(p);
        do_deallocate(p);
    }

private:
    chunk* deallocate_chunk_find(void* p) {
        const size_t chunk_length = num_blocks_ * block_size_;
        chunk* lo = dealloc_chunk_;
        chunk* hi = dealloc_chunk_;
        chunk* lo_bound = &chunks_.front();
        chunk* hi_bound = &chunks_.back() + 1;

        for (;;) {
            if (lo == nullptr && hi == nullptr)
                break;
            if (lo) {
                if (p >= lo->p_data_ && p < lo->p_data_ + chunk_length)
                    return lo;
                if (lo == lo_bound)
                    lo = nullptr;
                else
                    --lo;
            }

            if (hi) {
                if (p >= hi->p_data_ && p < hi->p_data_ + chunk_length)
                    return hi;
                if (++hi == hi_bound)
                    hi = nullptr;
            }
        }
        return nullptr;
    }

    void do_deallocate(void* p) {
        dealloc_chunk_->deallocate(p, block_size_);
        if (dealloc_chunk_->block_available_ == num_blocks_) {
            chunk& last_chunk = chunks_.back();
            if (&last_chunk == dealloc_chunk_) {
                if (chunks_.size() > 1 && dealloc_chunk_[-1].block_available_ == num_blocks_) {
                    last_chunk.release();
                    chunks_.pop_back();
                    alloc_chunk_ = &chunks_.front();
                }
                return ;
            }
            if (last_chunk.block_available_ == num_blocks_) {
                last_chunk.release();
                chunks_.pop_back();
                alloc_chunk_ = &chunks_.front();
            }
            else {
                std::swap(*dealloc_chunk_, last_chunk);
                alloc_chunk_ = &chunks_.back();
            }
        }
    }
};

template<typename T>
class loki_alloc {

public:

    static T* allocate() {
        return static_cast<T*>(allocator.allocate());
    }

    static void deallocate(T* p) {
        allocator.deallocate(static_cast<void*>(p));
    }

private:
    static fixed_allocator allocator;
    static unsigned char base_num_blocks_;
};

template <typename T>
unsigned char loki_alloc<T>::base_num_blocks_ = 5;

template <typename T>
fixed_allocator loki_alloc<T>::allocator(sizeof(T), base_num_blocks_);

} // namespace zephyr


#endif //ZEPHYR_LOKI_ALLOCATOR_H

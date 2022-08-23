//
// Created by Cu1 on 2022/8/23.
//

#ifndef ZEPHYR_LOKI_ALLOC_H
#define ZEPHYR_LOKI_ALLOC_H

#include <new>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace zephyr
{

struct chunk {

private:

    size_t first_available_block_;
    size_t block_available_;
    unsigned char* p_data_;

private:

    void reset(size_t block_size_, size_t blocks_) {
        block_available_ = blocks_;
        first_available_block_ = 0;
        unsigned char i = 0;
        unsigned char* p = p_data_;
        for (; i < blocks_; p += block_size_) {
            *p = ++i;
        }

    }

    void init(size_t block_size_, size_t blocks_) {
        p_data_ = new unsigned char [block_size_ * blocks_];
        reset(block_size_, blocks_);
    }

    void release() { delete[] p_data_; }


public:
    chunk(size_t block_size_,  size_t blocks_) {
        init(block_size_, blocks_);
    }

    void* allocate(size_t block_size_) {
        if (!block_available_) return nullptr;

        unsigned char* p_result_ =
                p_data_ + (block_size_ * first_available_block_);
        first_available_block_ = *p_result_;
        --block_available_;
        return p_result_;
    }

    void dellocate(void* p, size_t block_size_) {
        unsigned char* to_release_ = static_cast<unsigned char*>(p);
        *to_release_ = first_available_block_;
        first_available_block_ =
                static_cast<unsigned char>((to_release_ - p_data_) / block_size_);
        ++block_available_;
    }

    ~chunk() { release(); }

};

class loki_alloc_base {

private:


};


} // namespace zephyr


#endif //ZEPHYR_LOKI_ALLOC_H

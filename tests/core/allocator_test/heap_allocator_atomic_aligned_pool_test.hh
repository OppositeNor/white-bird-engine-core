/* Copyright 2025 OppositeNor

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef __WBE_ALLOCATOR_ATOMIC_ALIGNED_POOL_TEST_HH__
#define __WBE_ALLOCATOR_ATOMIC_ALIGNED_POOL_TEST_HH__

#include "core/allocator/heap_allocator_atomic_aligned_pool.hh"
#include "core/engine_core.hh"
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>

namespace WBE = WhiteBirdEngine;
constexpr size_t AAAPT_HEADER_SIZE = WBE::HeapAllocatorAtomicAlignedPool::HEADER_SIZE;

TEST(WBEAllocAtomicAlignedPoolTest, TraitTest) {
    ASSERT_TRUE(WBE::AllocatorTrait<WBE::HeapAllocatorAtomicAlignedPool>::IS_POOL);
    ASSERT_TRUE(WBE::AllocatorTrait<WBE::HeapAllocatorAtomicAlignedPool>::IS_LIMITED_SIZE);
    ASSERT_FALSE(WBE::AllocatorTrait<WBE::HeapAllocatorAtomicAlignedPool>::IS_GURANTEED_CONTINUOUS);
    ASSERT_FALSE(WBE::AllocatorTrait<WBE::HeapAllocatorAtomicAlignedPool>::IS_ALLOC_FIXED_SIZE);
    ASSERT_TRUE(WBE::AllocatorTrait<WBE::HeapAllocatorAtomicAlignedPool>::IS_ATOMIC);
}

TEST(WBEAllocAtomicAlignedPoolTest, SynchronizationTest) {
    std::unique_ptr<WBE::EngineCore> engine_core = std::make_unique<WBE::EngineCore>(0, nullptr, WBE::Directory({"test_env"}));
    const size_t pool_size = WBE_MiB(1);
    WBE::HeapAllocatorAtomicAlignedPool allocator(pool_size);
    const int num_threads = 8;
    const int allocs_per_thread = 128;
    std::vector<std::thread> threads;
    std::vector<WBE::MemID> mem_ids(num_threads * allocs_per_thread);
    std::atomic<int> alloc_count{0};

    auto alloc_func = [&](int thread_idx) {
        for (int i = 0; i < allocs_per_thread; ++i) {
            WBE::MemID mem = allocator.allocate(8, 8);
            ASSERT_NE(mem, WBE::MEM_NULL);
            mem_ids[thread_idx * allocs_per_thread + i] = mem;
            ++alloc_count;
        }
    };
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(alloc_func, t);
    }
    for (auto& th : threads) th.join();
    ASSERT_EQ(alloc_count, num_threads * allocs_per_thread);
    for (auto mem : mem_ids) {
        allocator.deallocate(mem);
    }
    ASSERT_EQ(allocator.get_remain_size(), pool_size);
}

TEST(WBEAllocAtomicAlignedPoolTest, SynchronizationGetPointerContentTest) {
    std::unique_ptr<WBE::EngineCore> engine_core = std::make_unique<WBE::EngineCore>(0, nullptr, WBE::Directory({"test_env"}));
    const size_t pool_size = WBE_MiB(1);
    WBE::HeapAllocatorAtomicAlignedPool allocator(pool_size);
    const int num_threads = 8;
    const int allocs_per_thread = 128;
    std::vector<std::thread> threads;
    std::vector<WBE::MemID> mem_ids(num_threads * allocs_per_thread);
    std::vector<int> values(num_threads * allocs_per_thread, 0);
    std::atomic<int> success_count{0};

    auto alloc_and_write_func = [&](int thread_idx) {
        for (int i = 0; i < allocs_per_thread; ++i) {
            int idx = thread_idx * allocs_per_thread + i;
            WBE::MemID mem = allocator.allocate(sizeof(int));
            ASSERT_NE(mem, WBE::MEM_NULL);
            mem_ids[idx] = mem;
            int* ptr = static_cast<int*>(allocator.get(mem));
            ASSERT_NE(ptr, nullptr);
            *ptr = idx * 10;
            values[idx] = *ptr;
            if (*ptr == idx * 10) ++success_count;
        }
    };
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(alloc_and_write_func, t);
    }
    for (auto& th : threads) th.join();
    ASSERT_EQ(success_count, num_threads * allocs_per_thread);
    // Verify values after all threads finished
    for (int idx = 0; idx < num_threads * allocs_per_thread; ++idx) {
        int* ptr = static_cast<int*>(allocator.get(mem_ids[idx]));
        ASSERT_NE(ptr, nullptr);
        ASSERT_EQ(*ptr, idx * 10);
        allocator.deallocate(mem_ids[idx]);
    }
    ASSERT_EQ(allocator.get_remain_size(), pool_size);
}

TEST(WBEAllocAtomicAlignedPoolTest, MixedAllocDeallocTest) {
    std::unique_ptr<WBE::EngineCore> engine_core = std::make_unique<WBE::EngineCore>(0, nullptr, WBE::Directory({"test_env"}));
    const size_t pool_size = WBE_MiB(1);
    WBE::HeapAllocatorAtomicAlignedPool allocator(pool_size);
    const int num_threads = 8;
    const int ops_per_thread = 256;
    std::vector<std::thread> threads;
    std::vector<std::vector<WBE::MemID>> thread_mem_ids(num_threads);
    std::atomic<int> alloc_count{0};
    std::atomic<int> dealloc_count{0};
    auto mix_func = [&](int thread_idx) {
        std::vector<WBE::MemID>& mems = thread_mem_ids[thread_idx];
        for (int i = 0; i < ops_per_thread; ++i) {
            if (i % 2 == 0) {
                WBE::MemID mem = allocator.allocate(8, 8);
                if (mem != WBE::MEM_NULL) {
                    mems.push_back(mem);
                    ++alloc_count;
                }
            } else if (!mems.empty()) {
                allocator.deallocate(mems.back());
                mems.pop_back();
                ++dealloc_count;
            }
        }
        // Deallocate any remaining allocations
        for (auto mem : mems) {
            allocator.deallocate(mem);
            ++dealloc_count;
        }
        mems.clear();
    };
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(mix_func, t);
    }
    for (auto& th : threads) th.join();
    ASSERT_EQ(allocator.get_remain_size(), pool_size);
    ASSERT_GE(alloc_count, num_threads * ops_per_thread / 2); // At least half ops are allocs
    ASSERT_EQ(dealloc_count, alloc_count);
}

TEST(WBEAllocAtomicAlignedPoolTest, MixedAllocDeallocGetTest) {
    std::unique_ptr<WBE::EngineCore> engine_core = std::make_unique<WBE::EngineCore>(0, nullptr, WBE::Directory({"test_env"}));
    const size_t pool_size = WBE_MiB(1);
    WBE::HeapAllocatorAtomicAlignedPool allocator(pool_size);
    const int num_threads = 8;
    const int ops_per_thread = 2048;
    std::vector<std::thread> threads;
    std::vector<std::vector<std::pair<WBE::MemID, int>>> thread_mem_ids(num_threads);
    auto mix_func = [&](int thread_idx) {
        std::vector<std::pair<WBE::MemID, int>>& mems = thread_mem_ids[thread_idx];
        for (int i = 0; i < ops_per_thread; ++i) {
            int op = std::rand() % 3;
            if (op == 0) {
                WBE::MemID mem = allocator.allocate(8, 8);
                ASSERT_TRUE(mem != WBE::MEM_NULL);
                int val = std::rand();
                void* ptr = allocator.get(mem);
                ASSERT_NE(ptr, nullptr);
                *static_cast<int*>(ptr) = val;
                mems.push_back(std::pair(mem, val));
            } else if (op == 1 && !mems.empty()) {
                allocator.deallocate(mems.back().first);
                mems.pop_back();
            } else if (op == 2 && !mems.empty()) {
                void* ptr = allocator.get(mems.back().first);
                ASSERT_NE(ptr, nullptr);
                ASSERT_EQ(*static_cast<int*>(ptr), mems.back().second);
            }
        }
        // Deallocate any remaining allocations
        for (auto mem : mems) {
            allocator.deallocate(mem.first);
        }
        mems.clear();
    };
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(mix_func, t);
    }
    for (auto& th : threads) th.join();
    ASSERT_EQ(allocator.get_remain_size(), pool_size);
}

TEST(WBEAllocAtomicAlignedPoolTest, SynchronizationDeallocationTest) {
    std::unique_ptr<WBE::EngineCore> engine_core = std::make_unique<WBE::EngineCore>(0, nullptr, WBE::Directory({"test_env"}));
    const size_t pool_size = WBE_MiB(1);
    WBE::HeapAllocatorAtomicAlignedPool allocator(pool_size);
    const int num_threads = 8;
    const int allocs_per_thread = 128;
    std::vector<WBE::MemID> mem_ids(num_threads * allocs_per_thread);
    // Allocate all memory in single thread for simplicity
    for (int i = 0; i < num_threads * allocs_per_thread; ++i) {
        WBE::MemID mem = allocator.allocate(8, 8);
        ASSERT_NE(mem, WBE::MEM_NULL);
        mem_ids[i] = mem;
    }
    std::vector<std::thread> threads;
    std::atomic<int> dealloc_count{0};
    auto dealloc_func = [&](int thread_idx) {
        // Each thread deallocates its own chunk
        for (int i = 0; i < allocs_per_thread; ++i) {
            int idx = thread_idx * allocs_per_thread + i;
            allocator.deallocate(mem_ids[idx]);
            ++dealloc_count;
        }
    };
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(dealloc_func, t);
    }
    for (auto& th : threads) th.join();
    ASSERT_EQ(dealloc_count, num_threads * allocs_per_thread);
    ASSERT_EQ(allocator.get_remain_size(), pool_size);
}

#endif

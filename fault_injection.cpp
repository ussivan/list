#include "fault_injection.h"
#include <cassert>
#include <iostream>
#include <vector>

#include <sys/mman.h>

namespace
{
    template <typename T>
    struct mmap_allocator
    {
        using value_type = T;

        mmap_allocator() = default;
        
        T* allocate(size_t n)
        {
            void* ptr = mmap(nullptr, n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
            if (ptr == MAP_FAILED)
                throw std::bad_alloc();
            return reinterpret_cast<T*>(ptr);
        }
        
        void deallocate(void* p, std::size_t n)
        {
            int r = munmap(p, n);
            if (r != 0)
                std::abort();
        }
        
    };

    thread_local bool fault_injection_enabled = false;
    thread_local std::vector<size_t, mmap_allocator<size_t> > skip_ranges;
    thread_local size_t error_index;
    thread_local size_t skip_index;
    thread_local bool fault_registred = false;
    
    void dump_state()
    {
        std::cout << "skip_ranges: {";
        if (!skip_ranges.empty())
        {
            std::cout << skip_ranges[0];
            for (size_t i = 1; i != skip_ranges.size(); ++i)
                std::cout << ", " << skip_ranges[i];
        }
        std::cout << "}\nerror_index: " << error_index << "\nskip_index: " << skip_index << '\n' << std::flush;
    }
}

bool should_inject_fault()
{
    if (!fault_injection_enabled)
        return false;

    assert(error_index <= skip_ranges.size());
    if (error_index == skip_ranges.size())
    {
        ++error_index;
        skip_ranges.push_back(0);
        fault_registred = true;
        return true;
    }

    assert(skip_index <= skip_ranges[error_index]);

    if (skip_index == skip_ranges[error_index])
    {
        ++error_index;
        skip_index = 0;
        fault_registred = true;
        return true;
    }

    ++skip_index;
    return false;
}

void fault_injection_point()
{
    if (should_inject_fault())
        throw injected_fault("injected fault");
}

void faulty_run(std::function<void ()> const& f)
{
    assert(!fault_injection_enabled);
    assert(skip_ranges.empty());
    fault_injection_enabled = true;
    for (;;)
    {
        try
        {
            f();
        }
        catch (...)
        {
            dump_state();
            skip_ranges.resize(error_index);
            ++skip_ranges.back();
            error_index = 0;
            skip_index = 0;
            assert(fault_registred);
            fault_registred = false;
            continue;
        }
        assert(!fault_registred);
        break;
    }
    fault_injection_enabled = false;
    skip_ranges.clear();
    error_index = 0;
    skip_index = 0;
    fault_registred = false;
}

void* operator new(std::size_t count)
{
    if (should_inject_fault())
        throw std::bad_alloc();
    
    return malloc(count);
}

void* operator new[](std::size_t count)
{
    if (should_inject_fault())
        throw std::bad_alloc();
    
    return malloc(count);
}

void operator delete(void* ptr) noexcept
{
    free(ptr);
}

void operator delete[](void* ptr) noexcept
{
    free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
    free(ptr);
}

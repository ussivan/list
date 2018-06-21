#pragma once

#include <functional>
#include <stdexcept>

struct injected_fault : std::runtime_error
{
    using runtime_error::runtime_error;
};

bool should_inject_fault();
void fault_injection_point();
void faulty_run(std::function<void ()> const& f);

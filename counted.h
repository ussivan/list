#pragma once

#include <set>
#include <gtest/gtest.h>

struct counted
{
    struct no_new_instances_guard;

    counted() = delete;

    counted(int data)
        : data(data)
    {
        auto p = instances.insert(this);
        EXPECT_TRUE(p.second);
    }

    counted(counted const& other)
        : data(other.data)
    {
        auto p = instances.insert(this);
        EXPECT_TRUE(p.second);
    }

    ~counted()
    {
        size_t n = instances.erase(this);
        EXPECT_EQ(1u, n);
    }

    counted& operator=(counted const& c)
    {
        EXPECT_TRUE(instances.find(this) != instances.end());

        data = c.data;
        return *this;
    }

    operator int() const
    {
        EXPECT_TRUE(instances.find(this) != instances.end());

        return data;
    }

    static void expect_no_instances()
    {
        EXPECT_EQ(0u, instances.size());
    }

private:
    int data;

    static std::set<counted const*> instances;
};

std::set<counted const*> counted::instances;

struct counted::no_new_instances_guard
{
    no_new_instances_guard()
        : old_instances(instances)
    {}
    
    no_new_instances_guard(no_new_instances_guard const&) = delete;
    no_new_instances_guard& operator=(no_new_instances_guard const&) = delete;

    ~no_new_instances_guard()
    {
        EXPECT_TRUE(old_instances == instances);
    }
    
private:
    std::set<counted const*> old_instances;
};

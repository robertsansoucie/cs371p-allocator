// -----------------
// TestAllocator.c++
// -----------------

// https://github.com/google/googletest
// https://github.com/google/googletest/blob/master/googletest/docs/primer.md
// https://github.com/google/googletest/blob/master/googletest/docs/advanced.md

// --------
// includes
// --------

#include <algorithm> // count
#include <cstddef>   // ptrdiff_t
#include <memory>    // allocator
#include <string>

#include "gtest/gtest.h"
#include <iostream>
#include "Allocator.hpp"

TEST(AllocatorFixture, test0) {
    using allocator_type = std::allocator<int>;
    using value_type     = typename allocator_type::value_type;
    using size_type      = typename allocator_type::size_type;
    using pointer        = typename allocator_type::pointer;

    allocator_type   x;
    const size_type  s = 10;
    const value_type v = 2;
    const pointer    b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(std::count(b, e, v), ptrdiff_t(s));
        while (b != e) {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}

TEST(AllocatorFixture, test1) {
    using allocator_type = my_allocator<int, 1000>;
    using value_type     = typename allocator_type::value_type;
    using size_type      = typename allocator_type::size_type;
    using pointer        = typename allocator_type::pointer;

    allocator_type   x;
    const size_type  s = 10;
    const value_type v = 2;
    const pointer    b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(std::count(b, e, v), ptrdiff_t(s));
        while (b != e) {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}

TEST(AllocatorFixture, test2) {
    using allocator_type = my_allocator<int, 1000>;
    using value_type     = typename allocator_type::value_type;
    using size_type      = typename allocator_type::size_type;
    using pointer        = typename allocator_type::pointer;

    allocator_type x;                                            // read-write
    ASSERT_EQ(x[0], 992);
}                                         // fix test

TEST(AllocatorFixture, test3) {
    using allocator_type = my_allocator<int, 1000>;
    using value_type     = typename allocator_type::value_type;
    using size_type      = typename allocator_type::size_type;
    using pointer        = typename allocator_type::pointer;

    const allocator_type x;                                      // read only
    ASSERT_EQ(x[0], 992);
}                                         // fix test

//valid tests
TEST(AllocatorFixture, test4) {
    my_allocator<double, 1000> x;
    ASSERT_EQ(x.isValid(), true);
}

TEST(AllocatorFixture, test5) {
    my_allocator<double, 1000> x;
    x.allocate(10);
    ASSERT_EQ(x.isValid(), true);
}

TEST(AllocatorFixture, test6) {
    my_allocator<double, 1000> x;
    double* p = x.allocate(10);
    int* firstSentinel = reinterpret_cast<int*>(p) - 1;
    *firstSentinel = -72;
    ASSERT_EQ(x.isValid(), false);
}

TEST(AllocatorFixture, test7) {
    my_allocator<double, 1000> x;
    double* p = x.allocate(10);
    int* firstSentinel = reinterpret_cast<int*>(p) - 1;
    *(firstSentinel + 1 + abs(*firstSentinel) / 4) = -72;
    ASSERT_EQ(x.isValid(), false);
}

TEST(AllocatorFixture, test8) {
    my_allocator<double, 1000> x;
    double* p = x.allocate(10);
    int* firstSentinel = reinterpret_cast<int*>(p) - 1;
    *firstSentinel = 80;
    *(firstSentinel + 1 + abs(*firstSentinel) / 4) = 80;
    ASSERT_EQ(x.isValid(), false);
}

TEST(AllocatorFixture, test9) {
    my_allocator<double, 1000> x;
    double* p = x.allocate(10);
    int* firstSentinel = reinterpret_cast<int*>(p) - 1;
    x.deallocate(p, 10);
    *firstSentinel = 1000;
    ASSERT_EQ(x.isValid(), false);
}

TEST(AllocatorFixture, test10) {
    my_allocator<double, 1000> x;
    x.allocate(124);
    ASSERT_EQ(x.isValid(), true);
}

TEST(AllocatorFixture, test11) {
    my_allocator<double, 1000> x;
    x.allocate(124);
    ASSERT_EQ(*x.begin(), -992);
}

TEST(AllocatorFixture, test12) {
    my_allocator<double, 1000> x;
    x.allocate(124);
    ASSERT_EQ(++x.begin() == x.end(), true);
}

TEST(AllocatorFixture, test13) {
    my_allocator<double, 1000> x;
    x.allocate(124);
    my_allocator<double, 1000>::iterator i = x.begin();
    ASSERT_EQ(&(*i) + 2 + abs(*i) / 4 == &(*x.end()), true);
}

TEST(AllocatorFixture, test14) {
    my_allocator<double, 1000> x;
    x.allocate(10);
    my_allocator<double, 1000>::iterator i = x.begin();
    int* address = &(*i);
    ++i;
    ASSERT_EQ(&(*i), address + 2 + abs(*address) / 4);
}

TEST(AllocatorFixture, test15) {
    my_allocator<double, 1000> x;
    x.allocate(10);
    my_allocator<double, 1000>::iterator i = x.begin();
    ++i;
    int* address = &(*i);
    --i;
    ASSERT_EQ(&(*i), address - 2 - abs(*(address - 1)) / 4);
}

TEST(AllocatorFixture, test16) {
    my_allocator<double, 1000> x;
    int random = rand() % 10 + 10;
    for(int n= 0; n < random; n++) {
        x.allocate(1);
    }
    my_allocator<double, 1000>::iterator i = x.begin();
    for(int n = 0; n < random; n++) {
        ASSERT_EQ(i == x.end(), false);
        ++i;
    }
    ++i;
    ASSERT_EQ(i == x.end(), true);

}

string printAllocator(my_allocator<double, 1000>& a) {
    my_allocator<double, 1000>::iterator b = a.begin();
    my_allocator<double, 1000>::iterator e = a.end();
    string s = "";
    while(b != e) {
        s += to_string(*b);
        ++b;
        if(b != e) {
            s += " ";
        }
    }
    return s;
}

TEST(AllocatorFixture, test17) {
    my_allocator<double, 1000> x;
    x.allocate(5);
    ASSERT_EQ(printAllocator(x), "-40 944");
}

TEST(AllocatorFixture, test18) {
    my_allocator<double, 1000> x;
    x.allocate(5);
    x.allocate(3);
    ASSERT_EQ(printAllocator(x), "-40 -24 912");
}

TEST(AllocatorFixture, test19) {
    my_allocator<double, 1000> x;
    double* p = x.allocate(5);
    x.allocate(3);
    x.deallocate(p, 5);
    ASSERT_EQ(printAllocator(x), "40 -24 912");
}

TEST(AllocatorFixture, test20) {
    my_allocator<double, 1000> x;
    double* p1 = x.allocate(5);
    double* p2 = x.allocate(3);
    x.allocate(3);
    x.deallocate(p1, 5);
    x.deallocate(p2, 3);
    ASSERT_EQ(printAllocator(x), "72 -24 880");
}

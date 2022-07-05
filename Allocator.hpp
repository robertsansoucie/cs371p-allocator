// -----------
// Allocator.h
// -----------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <iostream>

using namespace std;

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class my_allocator {
    // -----------
    // operator ==
    // -----------

    friend bool operator == (const my_allocator&, const my_allocator&) {
        return false;
    }                                                   // this is correct

    // -----------
    // operator !=
    // -----------

    friend bool operator != (const my_allocator& lhs, const my_allocator& rhs) {
        return !(lhs == rhs);
    }

public:
    // --------
    // typedefs
    // --------

    using      value_type = T;

    using       size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using       pointer   =       value_type*;
    using const_pointer   = const value_type*;

    using       reference =       value_type&;
    using const_reference = const value_type&;

public:
    // ---------------
    // iterator
    // over the blocks
    // ---------------

    class iterator {
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const iterator& first, const iterator& second) {
            return first._p == second._p;
        }

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const iterator& lhs, const iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        // ----
        // data
        // ----

        int* _p;

    public:
        // -----------
        // constructor
        // -----------

        iterator (int* p) {
            _p = p;
        }

        // ----------
        // operator *
        // ----------

        int& operator * () const {
            return *_p;
        }
        // -----------
        // operator ++
        // -----------

        iterator& operator ++ () {
            _p += 1 + abs(*_p) / 4 + 1;
            return *this;
        }

        // -----------
        // operator ++
        // -----------

        iterator operator ++ (int) {
            iterator x = *this;
            ++*this;
            return x;
        }

        // -----------
        // operator --
        // -----------

        iterator& operator -- () {
            int sizeOfPreviousBlock = abs(*(_p - 1));
            _p -= 1 + sizeOfPreviousBlock / 4 + 1;
            return *this;
        }

        // -----------
        // operator --
        // -----------

        iterator operator -- (int) {
            iterator x = *this;
            --*this;
            return x;
        }
    };

    // ---------------
    // const_iterator
    // over the blocks
    // ---------------

    class const_iterator {
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const const_iterator& first, const const_iterator& second) {
            return first._p == second._p;
        }

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const const_iterator& lhs, const const_iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        // ----
        // data
        // ----

        const int* _p;

    public:
        // -----------
        // constructor
        // -----------

        const_iterator (const int* p) {
            _p = p;
        }

        // ----------
        // operator *
        // ----------

        const int& operator * () const {
            return *_p;
        }

        // -----------
        // operator ++
        // -----------
        /**
        * Increments the iterator by moving over sentinels and the length of the block
        */

        const_iterator& operator ++ () {
            // <your code>
            _p += 1 + abs(*_p) / 4 + 1;
            return *this;
        }

        // -----------
        // operator ++
        // -----------

        const_iterator operator ++ (int) {
            const_iterator x = *this;
            ++*this;
            return x;
        }

        // -----------
        // operator --
        // -----------

        /**
        * Increments the iterator by checking the previous end sentinel and moving backwards over sentinels and the length of the previous block
        */

        const_iterator& operator -- () {
            int sizeOfPreviousBlock = *(_p - 1);
            _p -= 1 + sizeOfPreviousBlock / 4 + 1;
            return *this;
        }

        // -----------
        // operator --
        // -----------

        const_iterator operator -- (int) {
            const_iterator x = *this;
            --*this;
            return x;
        }
    };

private:
    // ----
    // data
    // ----

    char a[N];

    // -----
    // valid
    // -----

    /**
     * O(1) in space
     * O(n) in time
     * uses an iterator and check if it can reach the end, if all beginning sentinels match their end sentinels, and if no two free blocks are adjacent
     */
    bool valid () const {
        // <your code>
        //needs to make sure all the sentinels are kosher
        // <use iterators>
        const_iterator b = begin();
        const_iterator e = end();
        while(b != e) {
            const int* p = &(*b);
            if(*p != *(p + 1 + abs(*p) / 4)) {
                return false;
            }
            int oldValue = *p;
            ++b;
            if(b != e && oldValue > 0 && *b > 0) {
                return false;
            }
            if(&(*b) > &(*e)) {
                return false;
            }
        }
        return true;
    }

public:
    // -----------
    // constructor
    // -----------

    /**
     * O(1) in space
     * O(1) in time
     * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
     */
    my_allocator () {
        (*this)[0] = N - 8; // replace!
        (*this)[N - 4] = N - 8;
        // <your code>
        assert(valid());
    }

    my_allocator             (const my_allocator&) = default;
    ~my_allocator            ()                    = default;
    my_allocator& operator = (const my_allocator&) = default;

    bool isValid() {
        return valid();
    }

    /*
    bool hasAvailableBlockAfterAllocation(int* p, size_type s) {
        iterator i = begin();
        while(i != end()) {
            if(&(*i) == p) {
                if(*p - s * 8 >= sizeof(T) + 2 * sizeof(int)) {
                    return true;
                }
            }
            else {
                if(*i > 0 && *i >= (int)sizeof(T)) {
                    return true;
                }
            }
            ++i;
        }
        return false;
    }
    */

    // --------
    // allocate
    // --------

    /**
     * O(1) in space
     * O(n) in time
     * after allocation there must be enough space left for a valid block
     * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
     * choose the first block that fits
     * throw a bad_alloc exception, if n is invalid
     * finds the first available free block of a valid size and reapportions its sentinels into a new allocated block and the remaining free block
     */
    pointer allocate (size_type s) {
        int* currentBlock = reinterpret_cast<int*>(a);
        bool foundAvailableBlock = false;
        int newBlockSize = (int)s;
        while(!foundAvailableBlock) {
            if(currentBlock == reinterpret_cast<int*>(a + N)) {
                bad_alloc exception;
                throw exception;
                return nullptr;
            }
            if(*currentBlock > 0) {
                //cout << newBlockSize * 8 << " " << *currentBlock << endl;
                //cout << hasAvailableBlockAfterAllocation(currentBlock, newBlockSize) << endl;
                if(newBlockSize * 8 <= *currentBlock) { // && hasAvailableBlockAfterAllocation(currentBlock, newBlockSize)) {
                    //cout << "Found the block! " << *currentBlock << endl;
                    foundAvailableBlock = true;
                    int oldBlockSize = *currentBlock;
                    if(oldBlockSize - (newBlockSize * 8 + 8) < (int)sizeof(T)) {
                        currentBlock[0] = -1 * oldBlockSize;
                        currentBlock[oldBlockSize / 4 + 1] = -1 * oldBlockSize;
                    }
                    else {
                        currentBlock[0] = -8 * newBlockSize;
                        //cout << 0 << ": " << -8 * newBlockSize << endl;
                        currentBlock[1 + 2 * newBlockSize] = -8 * newBlockSize;
                        //cout << 1 + 2 * newBlockSize << ": " << -8 * newBlockSize << endl;
                        currentBlock[2 + 2 * newBlockSize] = oldBlockSize - (newBlockSize * 8 + 8);
                        //cout << 2 + 2 * newBlockSize << ": " << oldBlockSize - (newBlockSize * 8 + 8) << endl;
                        currentBlock[oldBlockSize / 4 + 1] = oldBlockSize - (newBlockSize * 8 + 8);
                    }
                    //cout << oldBlockSize / 4 + 1 << ": " << oldBlockSize - (newBlockSize * 8 + 8) << endl;
                }
                else {
                    currentBlock += 1 + *currentBlock / 4 + 1;
                }
            }
            else {
                currentBlock += 1 + abs(*currentBlock) / 4 + 1;
            }
        }
        assert(valid());
        return reinterpret_cast<T*>(currentBlock + 1);
    }

    // ---------
    // construct
    // ---------

    /**
     * O(1) in space
     * O(1) in time
     */
    void construct (pointer p, const_reference v) {
        new (p) T(v);                               // this is correct and exempt
        assert(valid());
    }                           // from the prohibition of new

    // ----------
    // deallocate
    // ----------

    /**
     * O(1) in space
     * O(1) in time
     * after deallocation adjacent free blocks must be coalesced
     * throw an invalid_argument exception, if p is invalid
     * determines the positions of sentinels based on whether adjacent blocks are free and sets their size based off the coalesced (or not) blocks
     */
    void deallocate (pointer p, size_type s) {
        int* blockHead = reinterpret_cast<int*>(p) - 1;
        if(*blockHead * -1 != (int)s * 8) {
            //invalid_argument exception("Bad arguments passed to deallocate");
            throw invalid_argument("Bad arguments passed to deallocate");
            return;
        }
        int* newBlockHead = blockHead;
        int* endBlockHead = blockHead + 1 + *blockHead / -4 + 1;
        int newSize = *newBlockHead * -1;
        if(reinterpret_cast<char*>(newBlockHead) != a && *(newBlockHead - 1) > 0) {
            newBlockHead -= 1 + *(newBlockHead - 1) / 4 + 1;
            newSize += 8 + *newBlockHead;
        }
        if(reinterpret_cast<char*>(endBlockHead) != &a[N] && *endBlockHead > 0) {
            newSize += 8 + *endBlockHead;
            endBlockHead += 1 + *endBlockHead / 4 + 1;
        }
        *newBlockHead = newSize;
        ++newBlockHead;
        while(newBlockHead != endBlockHead) {
            *newBlockHead = 0;
            ++newBlockHead;
        }
        *(endBlockHead - 1) = newSize;
        assert(valid());
    }

    // -------
    // destroy
    // -------

    /**
     * O(1) in space
     * O(1) in time
     */
    void destroy (pointer p) {
        p->~T();               // this is correct
        assert(valid());
    }

    // -----------
    // operator []
    // -----------

    /**
     * O(1) in space
     * O(1) in time
     */
    int& operator [] (int i) {
        return *reinterpret_cast<int*>(&a[i]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    const int& operator [] (int i) const {
        return *reinterpret_cast<const int*>(&a[i]);
    }

    // -----
    // begin
    // -----

    /**
     * O(1) in space
     * O(1) in time
     */
    iterator begin () {
        return iterator(&(*this)[0]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    const_iterator begin () const {
        return const_iterator(&(*this)[0]);
    }

    // ---
    // end
    // ---

    /**
     * O(1) in space
     * O(1) in time
     */
    iterator end () {
        return iterator(&(*this)[N]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    const_iterator end () const {
        return const_iterator(&(*this)[N]);
    }
};

#endif // Allocator_h

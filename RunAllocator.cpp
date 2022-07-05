// ----------------
// RunAllocator.c++
// ----------------

// --------
// includes
// --------

#include <iostream> // cin, cout
#include <string>
#include <vector>
#include "Allocator.hpp"

void printAllocator(my_allocator<double, 1000>& a) {
    my_allocator<double, 1000>::iterator b = a.begin();
    my_allocator<double, 1000>::iterator e = a.end();
    while(b != e) {
        cout << *b;
        ++b;
        if(b != e) {
            cout << " ";
        }
    }
    cout << endl;
}

// ----
// main
// ----

int main () {
    using namespace std;
    /*
    your code for the read eval print loop (REPL) goes here
    in this project, the unit tests will only be testing Allocator.hpp, not the REPL
    the acceptance tests will be testing the REPL
    */
    string s;
    getline(cin, s);
    int numberOfCases = stoi(s);
    getline(cin, s);
    for(int x = 0; x < numberOfCases; x++) {
        my_allocator<double, 1000> a;
        while(getline(cin, s)) {
            if(s == "") {
                break;
            }
            int size = stoi(s);
            if(size > 0) {
                a.allocate((size_t)size);
            }
            else {
                int index = size * -1 - 1;
                my_allocator<double, 1000>::iterator i = a.begin();
                while(index > 0) {
                    while(*i > 0) {
                        ++i;
                    }
                    --index;
                    ++i;
                }
                while(*i > 0) {
                    if(i == a.end()) {
                        //error
                    }
                    ++i;
                }
                a.deallocate(reinterpret_cast<double*>(&(*i) + 1), -1 * *i / 8);
            }
            //printAllocator(a);
        }
        printAllocator(a);
    }
    return 0;
}

.DEFAULT_GOAL := all
MAKEFLAGS     += --no-builtin-rules
SHELL         := bash

ASTYLE        := astyle
CHECKTESTDATA := checktestdata
CPPCHECK      := cppcheck
DOXYGEN       := doxygen
VALGRIND      := valgrind

ifeq ($(shell uname -s), Darwin)
    BOOST    := /usr/local/include/boost
    CXX      := g++-10
    CXXFLAGS := --coverage -pedantic -std=c++17 -O3 -I/usr/local/include -Wall -Wextra
    GCOV     := gcov-10
    GTEST    := /usr/local/include/gtest
    LDFLAGS  := -lgtest -lgtest_main
    LIB      := /usr/local/lib
else ifeq ($(shell uname -p), unknown)
    BOOST    := /usr/include/boost
    CXX      := g++
    CXXFLAGS := --coverage -pedantic -std=c++17 -O3 -Wall -Wextra
    GCOV     := gcov
    GTEST    := /usr/include/gtest
    LDFLAGS  := -lgtest -lgtest_main -pthread
    LIB      := /usr/lib
else
    BOOST    := /usr/include/boost
    CXX      := g++-9
    CXXFLAGS := --coverage -pedantic -std=c++17 -O3 -Wall -Wextra
    GCOV     := gcov-9
    GTEST    := /usr/local/include/gtest
    LDFLAGS  := -lgtest -lgtest_main -pthread
    LIB      := /usr/local/lib
endif

# run docker
docker:
	docker run -it -v $(PWD):/usr/gcc -w /usr/gcc gpdowning/gcc

# get git config
config:
	git config -l

# get git log
Allocator.log:
	git log > Allocator.log

# get git status
status:
	make clean
	@echo
	git branch
	git remote -v
	git status

# download files from the Allocator code repo
pull:
	make clean
	@echo
	git pull
	git status

# upload files to the Allocator code repo
push:
	make clean
	@echo
	git add .gitignore
	git add .gitlab-ci.yml
	git add Allocator.hpp
	-git add Allocator.log
	-git add html
	git add makefile
	git add README.md
	git add RunAllocator.cpp
	git add RunAllocator.ctd
	git add RunAllocator.in
	git add RunAllocator.out
	git add TestAllocator.cpp
	git commit -m "another commit"
	git push
	git status

# compile run harness
RunAllocator: Allocator.hpp RunAllocator.cpp
	-$(CPPCHECK) RunAllocator.cpp
	$(CXX) $(CXXFLAGS) RunAllocator.cpp -o RunAllocator

# compile test harness
TestAllocator: Allocator.hpp TestAllocator.cpp
	-$(CPPCHECK) TestAllocator.cpp
	$(CXX) $(CXXFLAGS) -DNDEBUG TestAllocator.cpp -o TestAllocator $(LDFLAGS)

# run/test files, compile with make all
FILES :=          \
    RunAllocator  \
    TestAllocator

# compile all
all: $(FILES)

# check integrity of input file
ctd-check:
	-$(CHECKTESTDATA) RunAllocator.ctd RunAllocator.in

# generate a random input file
ctd-generate:
	$(CHECKTESTDATA) -g RunAllocator.ctd RunAllocator.tmp

# execute run harness and diff with expected output
run: RunAllocator
	./RunAllocator < RunAllocator.in > RunAllocator.tmp
	-diff RunAllocator.tmp RunAllocator.out

# execute test harness
test: TestAllocator
	$(VALGRIND) ./TestAllocator
	$(GCOV) TestAllocator.cpp | grep -B 2 "cpp.gcov"

# clone the Allocator test repo
allocator-tests:
	git clone https://gitlab.com/gpdowning/cs371p-allocator-tests.git allocator-tests

# test files in the Allocator test repo
TFILES := `ls allocator-tests/*.in`

# execute run harness against a test in Allocator test repo and diff with expected output
allocator-tests/%: RunAllocator
	./RunAllocator < $@.in > RunAllocator.tmp
	-diff RunAllocator.tmp $@.out

# execute run harness against all tests in Allocator test repo and diff with expected output
tests: allocator-tests RunAllocator
	-for v in $(TFILES); do make $${v/.in/}; done

# auto format the code
format:
	$(ASTYLE) Allocator.hpp
	$(ASTYLE) RunAllocator.cpp
	$(ASTYLE) TestAllocator.cpp

# you must edit Doxyfile and
# set EXTRACT_ALL     to YES
# set EXTRACT_PRIVATE to YES
# set EXTRACT_STATIC  to YES
# create Doxfile
Doxyfile:
	$(DOXYGEN) -g

# create html directory
html: Doxyfile Allocator.hpp
	$(DOXYGEN) Doxyfile

# check files, check their existence with make check
CFILES :=          \
    .gitignore     \
    .gitlab-ci.yml \
    Allocator.log  \
    html

# check the existence of check files
check: $(CFILES)

# remove executables and temporary files
clean:
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f *.plist
	rm -f *.tmp
	rm -f RunAllocator
	rm -f TestAllocator

# remove executables, temporary files, and generated files
scrub:
	make clean
	rm -f  Allocator.log
	rm -f  Doxyfile
	rm -rf allocator-tests
	rm -rf html
	rm -rf latex

# output versions of all tools
versions:
	@echo "% shell uname -p"
	@echo  $(shell uname -p)
	@echo
	@echo "% shell uname -s"
	@echo  $(shell uname -s)
	@echo
	@echo "% which $(ASTYLE)"
	@which $(ASTYLE)
	@echo
	@echo "% $(ASTYLE) --version"
	@$(ASTYLE) --version
	@echo
	@echo "% which $(CHECKTESTDATA)"
	@which $(CHECKTESTDATA)
	@echo
	@echo "% $(CHECKTESTDATA) --version"
	@$(CHECKTESTDATA) --version
	@echo
	@echo "% which cmake"
	@which cmake
	@echo
	@echo "% cmake --version"
	@cmake --version
	@echo
	@echo "% which $(CPPCHECK)"
	@which $(CPPCHECK)
	@echo
	@echo "% $(CPPCHECK) --version"
	@$(CPPCHECK) --version
	@echo
	@$(CXX) --version
	@echo "% which $(DOXYGEN)"
	@which $(DOXYGEN)
	@echo
	@echo "% $(DOXYGEN) --version"
	@$(DOXYGEN) --version
	@echo
	@echo "% which $(CXX)"
	@which $(CXX)
	@echo
	@echo "% $(CXX) --version"
	@$(CXX) --version
	@echo
	@echo "% which $(GCOV)"
	@which $(GCOV)
	@echo
	@echo "% $(GCOV) --version"
	@$(GCOV) --version
	@echo
	@echo "% which git"
	@which git
	@echo
	@echo "% git --version"
	@git --version
	@echo
	@echo "% grep \"#define BOOST_LIB_VERSION \" $(BOOST)/version.hpp"
	@grep "#define BOOST_LIB_VERSION " $(BOOST)/version.hpp
	@echo
	@echo "% cat $(GTEST)/README"
	@cat $(GTEST)/README
	@echo
	@echo "% ls -al $(LIB)/libgtest*.a"
	@ls -al $(LIB)/libgtest*.a
	@echo
	@echo "% which make"
	@which make
	@echo
	@echo "% make --version"
	@make --version
ifneq ($(shell uname -s), Darwin)
	@echo
	@echo "% which $(VALGRIND)"
	@which $(VALGRIND)
	@echo
	@echo "% $(VALGRIND) --version"
	@$(VALGRIND) --version
endif
	@echo "% which vim"
	@which vim
	@echo
	@echo "% vim --version"
	@vim --version

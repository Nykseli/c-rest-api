#!/bin/bash

# all|unit|integration defines what test are going to be run
# if not assigned, it defaults to all
RUN=$1
echo $1
if [[ $RUN != "unit" && $RUN != "integration" ]]
then
    RUN="all"
fi

# Fail as soon as a test fails
set -e

function error() {
    echo ""
    echo "Not all the tests passed"
    kill -SIGTERM $U_PID &> /dev/null
}
trap error ERR

# Test names
TESTS=(json server)
# Integration tests
I_TESTS=(staticfiles simpleapi)

# Compile the library to make sure the changes are applied
make NAME=restapitest -j`grep processor /proc/cpuinfo | wc -l` &> /dev/null

# Find the project objects so we can test them
OBJECTS=build/restapitest.o

CC=gcc
TEST_CFLAGS="-std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -lcheck -lsubunit -lrt -lm"
UTEST_CFLAGS="-std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function"

if [[ $RUN == "unit" || $RUN == "all" ]]
then
    echo "------------- Running unit tests -------------"
    for T in "${TESTS[@]}"
    do
        $CC -o check_$T tests/check_$T.c $OBJECTS $TEST_CFLAGS &> /dev/null
        ./check_$T
        rm check_$T
    done
    echo "------------- Done running unit tests -------------"
    echo ""
fi

if [[ $RUN == "integration" || $RUN == "all" ]]
then
    echo "------------- Running integration tests -------------"
    for U in "${I_TESTS[@]}"
    do
        $CC -o i_$U tests/integration/i_$U.c $OBJECTS $TEST_CFLAGS &> /dev/null
        ./i_$U &> /dev/null &
        U_PID=$!
        python3 tests/integration/i_$U.py
        kill -SIGTERM $U_PID
        rm i_$U
    done
    echo "------------- Done running integration tests -------------"
    echo ""
fi

echo "All the tests passed"


cmake_minimum_required(VERSION 3.20)
project(sidh-sign VERSION 0.1 LANGUAGES C ASM)

enable_testing()

set(CMAKE_C_STANDARD 11)

set(DEBUG_FLAGS -O0)
set(RELEASE_FLAGS -Wextra -Wpedantic)
set(COMMON_FLAGS -Wall -fPIC)

if(APPLE)
    # REQUIRED for current ASM prime field arithmetic
    set(CONFIG_OS_FLAG "-D__APPLE__")
endif()

set(PRIMES
        377
        434
        503
        546
        610
        697
        751
        )

add_subdirectory(src)
add_subdirectory(tests)


cmake_minimum_required(VERSION 3.17)

project(cppkg C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third_party/cmake_repos/boost-cmake)

add_subdirectory(third_party/cmake_repos/grpc EXCLUDE_FROM_ALL)

# diff
add_library(diff STATIC diff/src/msg.cpp)
target_include_directories(diff PUBLIC .)
target_link_libraries(diff PUBLIC fmts)

# egrpc
add_library(egrpc STATIC egrpc/src/iclient_async.cpp)
target_include_directories(egrpc PUBLIC .)
target_link_libraries(egrpc PUBLIC logs error grpc++)

# error
add_library(error error/src/error.cpp)
target_include_directories(error PUBLIC .)
target_link_libraries(error PUBLIC fmts)

# estd
add_library(estd estd/src/strs.cpp)
target_include_directories(estd PUBLIC .)
target_link_libraries(estd PUBLIC logs)

# exam
add_library(exam exam/src/exam.cpp)
target_include_directories(exam PUBLIC .)
target_link_libraries(exam PUBLIC logs fmts)

# flag
add_library(flag flag/src/flag.cpp)
target_include_directories(flag PUBLIC .)
target_link_libraries(flag PUBLIC logs Boost::filesystem Boost::program_options)

# fmts
add_library(fmts fmts/src/fmts.cpp)
target_include_directories(fmts PUBLIC .)
target_link_libraries(fmts PUBLIC types)

# jobs
add_library(jobs INTERFACE)
target_include_directories(jobs INTERFACE .)
target_link_libraries(jobs INTERFACE logs)

# logs
add_library(logs logs/src/logs.cpp)
target_include_directories(logs PUBLIC .)
target_link_libraries(logs PUBLIC fmts)

# types
add_library(types INTERFACE)
target_include_directories(types INTERFACE .)

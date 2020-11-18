cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME Cppkg)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT_NAME} C CXX)

#### third_parties ####

# boost
if(NOT Boost_FOUND AND NOT TARGET Boost::boost)
    add_subdirectory(third_party/cmake_repos/boost-cmake EXCLUDE_FROM_ALL)
endif()

# grpc
if(NOT gRPC_FOUND AND NOT TARGET grpc++)
    add_subdirectory(third_party/cmake_repos/grpc EXCLUDE_FROM_ALL)
endif()

#### local libraries ####

# types
set(TYPES_LIB ${PROJECT_NAME}_types)
add_library(${TYPES_LIB} INTERFACE)
target_include_directories(${TYPES_LIB} INTERFACE .)

# fmts
set(FMTS_LIB ${PROJECT_NAME}_fmts)
add_library(${FMTS_LIB} fmts/src/fmts.cpp)
target_include_directories(${FMTS_LIB} PUBLIC .)
target_link_libraries(${FMTS_LIB} PUBLIC ${TYPES_LIB})

# diff
set(DIFF_LIB ${PROJECT_NAME}_diff)
add_library(${DIFF_LIB} STATIC diff/src/msg.cpp)
target_include_directories(${DIFF_LIB} PUBLIC .)
target_link_libraries(${DIFF_LIB} PUBLIC ${FMTS_LIB})

# ${LOGS_LIB}
set(LOGS_LIB ${PROJECT_NAME}_logs)
add_library(${LOGS_LIB} logs/src/logs.cpp)
target_include_directories(${LOGS_LIB} PUBLIC .)
target_link_libraries(${LOGS_LIB} PUBLIC ${FMTS_LIB})

# error
set(ERROR_LIB ${PROJECT_NAME}_error)
add_library(${ERROR_LIB} error/src/error.cpp)
target_include_directories(${ERROR_LIB} PUBLIC .)
target_link_libraries(${ERROR_LIB} PUBLIC ${FMTS_LIB})

# egrpc
set(EGRPC_LIB ${PROJECT_NAME}_egrpc)
add_library(${EGRPC_LIB} STATIC egrpc/src/iclient_async.cpp)
target_include_directories(${EGRPC_LIB} PUBLIC .)
target_link_libraries(${EGRPC_LIB} PUBLIC ${ERROR_LIB} ${LOGS_LIB} grpc++)

# estd
set(ESTD_LIB ${PROJECT_NAME}_estd)
add_library(${ESTD_LIB} estd/src/strs.cpp)
target_include_directories(${ESTD_LIB} PUBLIC .)
target_link_libraries(${ESTD_LIB} PUBLIC ${LOGS_LIB})

# exam
set(EXAM_LIB ${PROJECT_NAME}_exam)
add_library(${EXAM_LIB} exam/src/exam.cpp)
target_include_directories(${EXAM_LIB} PUBLIC .)
target_link_libraries(${EXAM_LIB} PUBLIC ${FMTS_LIB} ${LOGS_LIB})

# flag
set(FLAG_LIB ${PROJECT_NAME}_flag)
add_library(${FLAG_LIB} flag/src/flag.cpp)
target_include_directories(${FLAG_LIB} PUBLIC .)
target_link_libraries(${FLAG_LIB} PUBLIC ${LOGS_LIB} Boost::filesystem Boost::program_options)

# jobs
set(JOBS_LIB ${PROJECT_NAME}_jobs)
add_library(${JOBS_LIB} INTERFACE)
target_include_directories(${JOBS_LIB} INTERFACE .)
target_link_libraries(${JOBS_LIB} INTERFACE ${LOGS_LIB})

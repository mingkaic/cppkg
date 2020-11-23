cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME Cppkg)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT_NAME} C CXX)

#### third_parties ####

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(gRPC CONFIG REQUIRED)

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
add_library(${DIFF_LIB} diff/src/msg.cpp)
target_include_directories(${DIFF_LIB} PUBLIC .)
target_link_libraries(${DIFF_LIB} PUBLIC ${FMTS_LIB})

# logs
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
target_link_libraries(${EGRPC_LIB} PUBLIC ${ERROR_LIB} ${LOGS_LIB} gRPC::grpc++ gRPC::grpc++_unsecure)

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
target_link_libraries(${FLAG_LIB} PUBLIC ${LOGS_LIB} ${CONAN_LIBS_BOOST})

# jobs
set(JOBS_LIB ${PROJECT_NAME}_jobs)
add_library(${JOBS_LIB} INTERFACE)
target_include_directories(${JOBS_LIB} INTERFACE .)
target_link_libraries(${JOBS_LIB} INTERFACE ${LOGS_LIB})

#### tests ####

function(_test_cppkg)
    # fmts
    set(FMTS_TEST ${FMTS_LIB}_test)
    add_executable(${FMTS_TEST} fmts/test/main.cpp)
    target_link_libraries(${FMTS_TEST} ${CONAN_LIBS_GTEST} ${FMTS_LIB})
    add_test(NAME ${FMTS_TEST} COMMAND ${FMTS_TEST})

    # diff
    set(DIFF_TEST ${DIFF_LIB}_test)
    add_executable(${DIFF_TEST} diff/test/main.cpp)
    target_link_libraries(${DIFF_TEST} ${CONAN_LIBS_GTEST} ${DIFF_LIB})
    add_test(NAME ${DIFF_TEST} COMMAND ${DIFF_TEST})

    # logs
    set(LOGS_TEST ${LOGS_LIB}_test)
    add_executable(${LOGS_TEST} logs/test/main.cpp)
    target_link_libraries(${LOGS_TEST} ${CONAN_LIBS_GTEST} ${LOGS_LIB})
    add_test(NAME ${LOGS_TEST} COMMAND ${LOGS_TEST})

    # error
    set(ERROR_TEST ${ERROR_LIB}_test)
    add_executable(${ERROR_TEST} error/test/main.cpp)
    target_link_libraries(${ERROR_TEST} ${CONAN_LIBS_GTEST} ${ERROR_LIB})
    add_test(NAME ${ERROR_TEST} COMMAND ${ERROR_TEST})

    # egrpc
    find_program(_PROTOBUF_PROTOC protoc ${CONAN_BIN_DIRS_PROTOBUF} NO_DEFAULT_PATH)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

    get_filename_component(EGRPC_TEST_PROTO "${CMAKE_CURRENT_SOURCE_DIR}/egrpc/test/test.proto" ABSOLUTE)
    set(EGRPC_TEST_PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.pb.cc")
    set(EGRPC_TEST_PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.pb.h")
    set(EGRPC_TEST_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.grpc.pb.cc")
    set(EGRPC_TEST_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.grpc.pb.h")
    add_custom_command(
        OUTPUT "${EGRPC_TEST_PROTO_SRCS}" "${EGRPC_TEST_PROTO_HDRS}" "${EGRPC_TEST_GRPC_SRCS}" "${EGRPC_TEST_GRPC_HDRS}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${CMAKE_CURRENT_SOURCE_DIR}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${EGRPC_TEST_PROTO}"
        DEPENDS "${EGRPC_TEST_PROTO}")
    add_library(EGRPC_TEST_PROTO_OBJ OBJECT ${EGRPC_TEST_PROTO_SRCS} ${EGRPC_TEST_GRPC_SRCS})
    include_directories("${CMAKE_CURRENT_BINARY_DIR}")

    set(EGRPC_TEST ${EGRPC_LIB}_test)
    add_executable(${EGRPC_TEST} egrpc/test/main.cpp $<TARGET_OBJECTS:EGRPC_TEST_PROTO_OBJ>)
    target_link_libraries(${EGRPC_TEST} ${CONAN_LIBS_GTEST} ${EGRPC_LIB} ${ESTD_LIB} ${EXAM_LIB})
    add_test(NAME ${EGRPC_TEST} COMMAND ${EGRPC_TEST})

    # estd
    set(ESTD_TEST ${ESTD_LIB}_test)
    add_executable(${ESTD_TEST}
        estd/test/test_cast.cpp
        estd/test/test_config.cpp
        estd/test/test_contain.cpp
        estd/test/test_hashlist.cpp
        estd/test/test_range.cpp
        estd/test/test_strs.cpp
        estd/test/main.cpp)
    target_link_libraries(${ESTD_TEST} ${CONAN_LIBS_GTEST} ${ESTD_LIB} ${EXAM_LIB})
    add_test(NAME ${ESTD_TEST} COMMAND ${ESTD_TEST})

    # exam
    set(EXAM_TEST ${EXAM_LIB}_test)
    add_executable(${EXAM_TEST} exam/test/main.cpp)
    target_link_libraries(${EXAM_TEST} ${CONAN_LIBS_GTEST} ${EXAM_LIB})
    add_test(NAME ${EXAM_TEST} COMMAND ${EXAM_TEST})

    # flag
    set(FLAG_TEST ${FLAG_LIB}_test)
    add_executable(${FLAG_TEST} flag/test/main.cpp)
    target_link_libraries(${FLAG_TEST} ${CONAN_LIBS_GTEST} ${FLAG_LIB})
    add_test(NAME ${FLAG_TEST} COMMAND ${FLAG_TEST})

    # jobs
    set(JOBS_TEST ${JOBS_LIB}_test)
    add_executable(${JOBS_TEST} jobs/test/main.cpp)
    target_link_libraries(${JOBS_TEST} ${CONAN_LIBS_GTEST} ${JOBS_LIB})
    add_test(NAME ${JOBS_TEST} COMMAND ${JOBS_TEST})
endfunction()
_test_cppkg()

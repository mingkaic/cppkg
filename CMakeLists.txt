cmake_minimum_required(VERSION 3.17)

project(cppkg C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PACKAGE_TESTS "Build the tests" ON)

set(cppkg_INSTALL_default ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   set(cppkg_INSTALL_default OFF)
endif()
set(cppkg_INSTALL ${cppkg_INSTALL_default} CACHE BOOL "Generate installation target")
set(cppkg_INSTALL_BINDIR "bin" CACHE STRING "Installation directory for executables")
set(cppkg_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
set(cppkg_INSTALL_INCLUDEDIR "include" CACHE STRING "Installation directory for headers")
set(cppkg_INSTALL_CMAKEDIR "lib/cmake/${PACKAGE_NAME}" CACHE STRING "Installation directory for cmake config files")

#### third_parties ####

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(gRPC CONFIG REQUIRED)

#### local libraries ####

include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
    $<INSTALL_INTERFACE:.>
)

# arrs
add_library(arrs INTERFACE)

# diff
add_library(diff cppkg/diff/src/msg.cc)
target_link_libraries(diff PUBLIC ${CONAN_LIBS_CISAB})

# egrpc
add_library(egrpc STATIC cppkg/egrpc/src/iclient_async.cc)
target_link_libraries(egrpc PUBLIC error ${CONAN_LIBS_CISAB} gRPC::grpc++ gRPC::grpc++_unsecure)

# error
add_library(error cppkg/error/src/error.cc)
target_link_libraries(error PUBLIC ${CONAN_LIBS_CISAB})

# estd
add_library(estd cppkg/estd/src/strs.cc)
target_link_libraries(estd PUBLIC ${CONAN_LIBS_CISAB})

# flag
add_library(flag cppkg/flag/src/flag.cc)
target_link_libraries(flag PUBLIC ${CONAN_LIBS_CISAB} ${CONAN_LIBS_BOOST})

# jobs
add_library(jobs INTERFACE)
target_link_libraries(jobs INTERFACE ${CONAN_LIBS_CISAB})

# numbers
add_library(numbers cppkg/numbers/src/decimal.cc)
target_link_libraries(numbers PUBLIC ${CONAN_LIBS_CISAB})

# units
add_library(units cppkg/units/src/memory.cc cppkg/units/src/time.cc)
target_link_libraries(units PUBLIC ${CONAN_LIBS_CISAB})

#### install ####
if(cppkg_INSTALL)
    foreach(_HDR
        cppkg/arrs/arrs.h
        cppkg/arrs/list.h
        cppkg/arrs/vecs.h
        cppkg/diff/diff.h
        cppkg/diff/format.h
        cppkg/diff/msg.h
        cppkg/diff/myers_diff.h
        cppkg/egrpc/client_async.h
        cppkg/egrpc/client_async_stream.h
        cppkg/egrpc/egrpc.h
        cppkg/egrpc/iclient.h
        cppkg/egrpc/iclient_async.h
        cppkg/egrpc/icqueue.h
        cppkg/egrpc/iserver_async.h
        cppkg/egrpc/server_async.h
        cppkg/egrpc/server_async_stream.h
        cppkg/error/error.h
        cppkg/estd/algorithm.h
        cppkg/estd/cast.h
        cppkg/estd/config.h
        cppkg/estd/contain.h
        cppkg/estd/estd.h
        cppkg/estd/hashlist.h
        cppkg/estd/range.h
        cppkg/estd/strs.h
        cppkg/flag/flag.h
        cppkg/flag/flagset.h
        cppkg/jobs/jobs.h
        cppkg/jobs/managed_job.h
        cppkg/jobs/scope_guard.h
        cppkg/jobs/sequence.h
        cppkg/numbers/decimal.h
        cppkg/numbers/numbers.h
        cppkg/units/memory.h
        cppkg/units/time.h
        cppkg/units/units.h
    )
        get_filename_component(_PATH ${_HDR} PATH)
        install(FILES ${_HDR} DESTINATION "${cppkg_INSTALL_INCLUDEDIR}/${_PATH}")
    endforeach()

    set(TargetFile ${PROJECT_NAME}Targets)
    install(TARGETS
        arrs
        diff
        egrpc
        error
        estd
        flag
        jobs
        numbers
        units
        EXPORT ${TargetFile}
        RUNTIME DESTINATION ${cppkg_INSTALL_BINDIR}
        LIBRARY DESTINATION ${cppkg_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${cppkg_INSTALL_LIBDIR}
    )
    install(EXPORT ${TargetFile}
        DESTINATION ${cppkg_INSTALL_CMAKEDIR}
        NAMESPACE ${PROJECT_NAME}::
    )
endif()

#### tests ####

if(PACKAGE_TESTS)
    enable_testing()
endif()

# arrs
set(ARRS_TEST arrs_test)
add_executable(${ARRS_TEST} cppkg/arrs/test/main.cc)
target_link_libraries(${ARRS_TEST} ${CONAN_LIBS_VERUM} arrs)
add_test(NAME ${ARRS_TEST} COMMAND ${ARRS_TEST})

# diff
set(DIFF_TEST diff_test)
add_executable(${DIFF_TEST} cppkg/diff/test/main.cc)
target_link_libraries(${DIFF_TEST} ${CONAN_LIBS_GTEST} diff)
add_test(NAME ${DIFF_TEST} COMMAND ${DIFF_TEST})

# egrpc
find_program(_PROTOBUF_PROTOC protoc ${CONAN_BIN_DIRS_PROTOBUF} NO_DEFAULT_PATH)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

get_filename_component(EGRPC_TEST_PROTO "${CMAKE_CURRENT_SOURCE_DIR}/egrpc/pbtest/test.proto" ABSOLUTE)
set(EGRPC_TEST_PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/pbtest/test.pb.cc")
set(EGRPC_TEST_PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/pbtest/test.pb.h")
set(EGRPC_TEST_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/pbtest/test.grpc.pb.cc")
set(EGRPC_TEST_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/pbtest/test.grpc.pb.h")
add_custom_command(
OUTPUT "${EGRPC_TEST_PROTO_SRCS}" "${EGRPC_TEST_PROTO_HDRS}" "${EGRPC_TEST_GRPC_SRCS}" "${EGRPC_TEST_GRPC_HDRS}"
COMMAND ${_PROTOBUF_PROTOC}
ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${CMAKE_CURRENT_SOURCE_DIR}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${EGRPC_TEST_PROTO}"
DEPENDS "${EGRPC_TEST_PROTO}")
add_library(egrpc_test_proto_obj OBJECT ${EGRPC_TEST_PROTO_SRCS} ${EGRPC_TEST_GRPC_SRCS})
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(EGRPC_TEST egrpc_test)
add_executable(${EGRPC_TEST} cppkg/egrpc/test/main.cc $<TARGET_OBJECTS:egrpc_test_proto_obj>)
target_link_libraries(${EGRPC_TEST} egrpc estd ${CONAN_LIBS_VERUM})
add_test(NAME ${EGRPC_TEST} COMMAND ${EGRPC_TEST})

# error
set(ERROR_TEST error_test)
add_executable(${ERROR_TEST} cppkg/error/test/main.cc)
target_link_libraries(${ERROR_TEST} ${CONAN_LIBS_GTEST} error)
add_test(NAME ${ERROR_TEST} COMMAND ${ERROR_TEST})

# estd
set(ESTD_TEST estd_test)
add_executable(${ESTD_TEST}
    cppkg/estd/test/test_cast.cc
    cppkg/estd/test/test_config.cc
    cppkg/estd/test/test_contain.cc
    cppkg/estd/test/test_hashlist.cc
    cppkg/estd/test/test_range.cc
    cppkg/estd/test/test_strs.cc
    cppkg/estd/test/main.cc)
target_link_libraries(${ESTD_TEST} estd ${CONAN_LIBS_VERUM})
add_test(NAME ${ESTD_TEST} COMMAND ${ESTD_TEST})

# flag
set(FLAG_TEST flag_test)
add_executable(${FLAG_TEST} cppkg/flag/test/main.cc)
target_link_libraries(${FLAG_TEST} ${CONAN_LIBS_GTEST} flag)
add_test(NAME ${FLAG_TEST} COMMAND ${FLAG_TEST})

# jobs
set(JOBS_TEST jobs_test)
add_executable(${JOBS_TEST} cppkg/jobs/test/main.cc)
target_link_libraries(${JOBS_TEST} ${CONAN_LIBS_GTEST} jobs)
add_test(NAME ${JOBS_TEST} COMMAND ${JOBS_TEST})

# numbers
set(NUMBERS_TEST numbers_test)
add_executable(${NUMBERS_TEST} cppkg/numbers/test/main.cc)
target_link_libraries(${NUMBERS_TEST} ${CONAN_LIBS_GTEST} numbers)
add_test(NAME ${NUMBERS_TEST} COMMAND ${NUMBERS_TEST})

# units
set(UNITS_TEST units_test)
add_executable(${UNITS_TEST} cppkg/units/test/main.cc)
target_link_libraries(${UNITS_TEST} ${CONAN_LIBS_GTEST} units)
add_test(NAME ${UNITS_TEST} COMMAND ${UNITS_TEST})

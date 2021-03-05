cmake_minimum_required(VERSION 3.17)
project(RedisNative)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set (CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")

    set(CMAKE_CXX_FLAGS_RELEASE "/O2sy")

    add_definitions(-DBOOST_ALL_NO_LIB -D_WIN32_WINNT=0x0601)
    add_definitions(-DCMAKE_GENERATOR_PLATFORM=x64)
    add_definitions(-DCMAKE_TOOLCHAIN_FILE=64bit.toolchain)

    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_DEBUG_LIBS OFF)

set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)

set(SOURCE_REDIS_NATIVE
        src/dllmain.cpp
        src/component/src/ComponentBaseImplExtern.cpp
        src/component/src/AddInNative.cpp
        src/component/src/extension.cpp
        src/component/src/RedisNative.cpp
        src/redis_lib/include/windows_cert.h)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

if (BOOST_FOUND)
    message(STATUS "BOOST INCLUDE DIRS: " ${Boost_INCLUDE_DIRS})
    message(STATUS "BOOST LIBRARY DIRS: " ${Boost_LIBRARY_DIRS})
    message(STATUS "BOOST VERSION: " ${Boost_VERSION})
endif()

if (OpenSSL_FOUND)
    message(STATUS "OPENSSL INCLUDE DIRS: " ${OPENSSL_INCLUDE_DIR})
    message(STATUS "OPENSSL CRYPTO LIBRARY: " ${OPENSSL_CRYPTO_LIBRARY})
    message(STATUS "OPENSSL SSL LIBRARY: " ${OPENSSL_SSL_LIBRARY})
    message(STATUS "OPENSSL VERSION: " ${OPENSSL_VERSION})
endif()

include_directories(${Boost_INCLUDE_DIRS}  ${OPENSSL_INCLUDE_DIR})

add_library(RedisLib STATIC src/redis_lib/src/redis_connection.cpp src/redis_lib/src/redis_object.cpp src/redis_lib/src/details.cpp)
target_link_libraries(RedisLib OpenSSL::SSL OpenSSL::Crypto)
if (MSVC)
    target_link_libraries(RedisLib crypt32)
endif()

add_library(RedisNative SHARED ${SOURCE_REDIS_NATIVE})
target_include_directories(RedisNative PRIVATE ./include src/redis_lib/include)
target_link_libraries(RedisNative RedisLib)

if (MSVC)
    if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
       set_property(TARGET RedisLib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
       set_property(TARGET RedisNative PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        set_property(TARGET RedisLib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
        set_property(TARGET RedisNative PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endif()

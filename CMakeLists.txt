cmake_minimum_required(VERSION 3.17.5)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Put path to NDK such as PATH//TO//NDK
file (STRINGS "ndkpath.txt" CMAKE_ANDROID_NDK)

# TODO: Uncomment when CLion supports CMake 3.20.x+
#set(CMAKE_SYSTEM_NAME Android)
#set(ANDROID_NATIVE_API_LEVEL 25)
#set(ANDROID_ABI arm64-v8a)
#set(ANDROID_TARGET_ARCH arm64-v8a)
#set(CMAKE_ANDROID_STL_TYPE c++_static)


#set(CMAKE_C_COMPILER_TARGET aarch64-none-linux-android24)
#set(CMAKE_C_COMPILER ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe)
#set(CMAKE_CXX_COMPILER_TARGET aarch64-none-linux-android24)
#set(CMAKE_CXX_COMPILER ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe)
#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake)


# set the project name
project(Chroma) #Modify this to your project name



message("Using NDK ${CMAKE_ANDROID_NDK}")

include_directories(include)
include_directories(extern)
include_directories(shared)
include_directories(extern/libil2cpp/il2cpp/libil2cpp)
include_directories(extern/codegen/include)
include_directories(extern/beatsaber-hook/shared)
include_directories(extern/modloader/shared)
include_directories(extern/custom-json-data/shared)
include_directories(extern/custom-types/shared)
include_directories(${CMAKE_ANDROID_NDK})
include_directories(.)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions("VERSION=\"0.1.0\"") #Modify this if you use it
add_compile_definitions("__GNUC__")
add_compile_definitions("__aarch64__")

file(GLOB_RECURSE SRC src/*.cpp src/**/*.cpp include/*.hpp include/*/*.hpp shared/*.hpp shared/*/*.hpp src/*/*/*.cpp)
add_library(Chroma SHARED ${SRC}) # Is this necessary? Not sure, have it there anyways
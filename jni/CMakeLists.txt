cmake_minimum_required(VERSION 3.10.2)
project("edgedemo")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/../OpenCV-android-sdk/sdk/native/jni")

# Find OpenCV
find_package(OpenCV REQUIRED CONFIG)

# Debug info
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")

# Add shared library with all your native source files
add_library(
    native-lib
    SHARED
    native-lib.cpp
    edge_detector.cpp
    ../gl/gl_renderer.cpp   
)

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../gl
)

# Find Android log library
find_library(log-lib log)

target_link_libraries(
    native-lib
    ${OpenCV_LIBS}
    ${log-lib}
    EGL
    GLESv2
)

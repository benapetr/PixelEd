# So that MacOS builds are working on older versions as well
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)

project(PixelEditor)
cmake_minimum_required (VERSION 2.8.0)
if(WIN32)
    if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.0)
        add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
    endif()
endif()

if(WIN32)
    if(MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
endif()

# Settings
option(QT5_BUILD "Build with Qt5" true)
option(EXTREME_DEBUG "Enable some resource expensive and slowing debug info" false)
option(UNIT_TESTS "Enable unit tests" false)
option(OPENGL "Use OpenGL as default rendering engine" true)

# Configuration
if (UNIT_TESTS)
    SET(QT_USE_QTTEST TRUE)
endif()
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Unit test
if (UNIT_TESTS)
    add_subdirectory (tests)
endif()

if (OPENGL)
    ADD_DEFINITIONS(-DSCORCHE_GL)
endif()

if (EXTREME_DEBUG)
    ADD_DEFINITIONS(-DPE_DEBUG)
endif()

# Libraries
add_subdirectory ("PixelEngine/src/PixelEngine")
add_subdirectory ("PixelEd")


# Packaging code
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Petr Bena")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PixelEditor")
SET(CPACK_PACKAGE_VENDOR "Petr Bena")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
INCLUDE(CPack)

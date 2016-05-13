if(NOT TIZEN_SDK)
    message(SEND_ERROR "Set tizen sdk path: $TIZEN_SDK or -DTIZEN_SDK")
    return()
endif()

#include(${CMAKE_SOURCE_DIR}/toolchains/tizen.toolchain.cmake)

# set for test in other cmake files
set(PLATFORM_TIZEN ON)

# global compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -fPIC -fPIE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -pie")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -lc++ -lc++abi")
endif()

if (CMAKE_COMPILER_IS_GNUCC)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCC_VERSION)
  string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
  list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
  list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

  message(STATUS "Using gcc ${GCC_VERSION}")
  if (GCC_VERSION VERSION_GREATER 5.1)
    message(STATUS "USE CXX11_ABI")
    add_definitions("-D_GLIBCXX_USE_CXX11_ABI=1")
  endif()
endif()

check_unsupported_compiler_version()

# compile definitions (adds -DPLATFORM_LINUX)
set(CORE_COMPILE_DEFS PLATFORM_TIZEN PLATFORM_LINUX)

if (USE_EXTERNAL_LIBS)
include(${EXTERNAL_LIBS_DIR}/core-dependencies.cmake)
include(${EXTERNAL_LIBS_DIR}/glfw.cmake)
else()
add_subdirectory(${PROJECT_SOURCE_DIR}/external)
endif()

# load core library
add_subdirectory(${PROJECT_SOURCE_DIR}/core)

add_custom_target(copy_tizen_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/core/resources/shaders
    ${PROJECT_SOURCE_DIR}/tizen/demo/res/shaders

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/core/resources/img
    ${PROJECT_SOURCE_DIR}/tizen/demo/res/img

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/core/resources/fonts
    ${PROJECT_SOURCE_DIR}/tizen/demo/res/fonts)

add_dependencies(${CORE_LIBRARY} copy_tizen_resources)

cmake_minimum_required(VERSION 3.10.2)

project(funcharmonie VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Change path from /src if needed, or add more directories
file(GLOB_RECURSE sources
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        )

add_library(CxxMidi INTERFACE)
add_library(CxxMidi::CxxMidi ALIAS CxxMidi)

#find_package(lsp1.2.0 EXACT REQUIRED)
#add_library(funcharmonie PUBLIC lsp::lsp ALIAS lsp)

target_include_directories(CxxMidi
  INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

include(CMakePackageConfigHelpers)

set(INCLUDE_INSTALL_DIR include)
set(LIB_INSTALL_DIR lib)
set(SYSCONFIG_INSTALL_DIR etc)


find_package(Threads REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 COMPONENTS Gui Widgets REQUIRED)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  find_package(ALSA REQUIRED)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  # todo
endif()


add_executable(funcharmonie ${sources})

target_compile_options(funcharmonie PRIVATE -g -Wall -pedantic -Wextra)
target_compile_features(funcharmonie PRIVATE cxx_std_23)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_include_directories(funcharmonie PRIVATE ${ALSA_INCLUDE_DIRS})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  # todo
endif()

target_link_libraries(funcharmonie
  CxxMidi::CxxMidi
  Threads::Threads
  Qt6::Widgets
  lsp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries(funcharmonie ${ALSA_LIBRARIES})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  # todo
endif()

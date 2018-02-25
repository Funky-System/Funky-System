cmake_minimum_required(VERSION 3.5)
project(Funky-System C)
set(CMAKE_C_STANDARD 11)

include(ExternalProject)
INCLUDE(FindPkgConfig)

set(VM_ARCH_BITS 32)
set(VM_NATIVE_MALLOC 0)
set(CMAKE_BUILD_TYPE Debug)


## VIRTUAL MACHINE
ExternalProject_Add(
        funky-vm
        GIT_REPOSITORY https://github.com/Funky-System/Funky-VM.git
        GIT_TAG master
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/funky-vm
        # Disable install step
        INSTALL_COMMAND ""

        CMAKE_ARGS -DVM_ARCH_BITS=${VM_ARCH_BITS} -DVM_NATIVE_MALLOC=${VM_NATIVE_MALLOC} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)
ExternalProject_Get_Property(funky-vm source_dir binary_dir)
add_library(libfunky-vm IMPORTED STATIC GLOBAL)
add_dependencies(libfunky-vm funky-vm)
set_target_properties(libfunky-vm PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/libfunky-vm.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
include_directories("${source_dir}/include")


## FONTS
add_custom_target(
        fonts
        python ${CMAKE_CURRENT_SOURCE_DIR}/fonts/font_builder.py ${CMAKE_CURRENT_BINARY_DIR}
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


## RUNTIME
add_custom_target(runtime bash ${CMAKE_CURRENT_SOURCE_DIR}/runtime/build.sh ${CMAKE_CURRENT_SOURCE_DIR}/runtime ${CMAKE_CURRENT_BINARY_DIR} ${VM_ARCH_BITS})


## FUNKY
add_definitions(-DVM_ARCH_BITS=${VM_ARCH_BITS})
add_definitions(-DVM_NATIVE_MALLOC=${VM_NATIVE_MALLOC})
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
add_executable(funky main.c display.c display.h input.c input.h funky_system.h vm.c vm.h runtime/draw.c font.h runtime/input.c runtime/timer.c runtime/clipboard.c)
set_property(SOURCE display.c APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/*.fnt.h)
add_dependencies(funky runtime fonts)
target_link_libraries(funky libfunky-vm m ${SDL2_LIBRARIES})

cmake_minimum_required(VERSION 3.9)
project(Funky-System C)

include(ExternalProject)

set(CMAKE_C_STANDARD 11)

ExternalProject_Add(
        funky-as
        GIT_REPOSITORY https://github.com/Funky-System/Funky-Assembler.git
        GIT_TAG master
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/funky-as
        # Disable install step
        INSTALL_COMMAND ""
)
ExternalProject_Get_Property(funky-as source_dir binary_dir)
add_library(libfunky-as IMPORTED STATIC GLOBAL)
add_dependencies(libfunky-as funky-as)
set_target_properties(libfunky-as PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/libfunky-as.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
include_directories("${source_dir}/include")

ExternalProject_Add(
        funky-vm
        GIT_REPOSITORY https://github.com/Funky-System/Funky-VM.git
        GIT_TAG master
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/funky-vm
        # Disable install step
        INSTALL_COMMAND ""
)
ExternalProject_Get_Property(funky-vm source_dir binary_dir)
add_library(libfunky-vm IMPORTED STATIC GLOBAL)
add_dependencies(libfunky-vm funky-vm)
set_target_properties(libfunky-vm PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/libfunky-vm.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
include_directories("${source_dir}/include")

INCLUDE(FindPkgConfig)

add_custom_target(kernel wickedc ${CMAKE_CURRENT_SOURCE_DIR}/runtime/kernel.wckd -o ${CMAKE_CURRENT_BINARY_DIR}/kernel.funk)
add_custom_target(draw wickedc ${CMAKE_CURRENT_SOURCE_DIR}/runtime/draw.wckd -o ${CMAKE_CURRENT_BINARY_DIR}/draw.funk)
add_custom_target(runtime DEPENDS kernel draw)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
add_executable(funky main.c display.c display.h input.c input.h funky_system.h vm.c vm.h runtime/draw.c)
add_dependencies(funky runtime)
target_link_libraries(funky libfunky-as libfunky-vm ${SDL2_LIBRARIES})


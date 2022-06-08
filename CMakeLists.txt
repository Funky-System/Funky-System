cmake_minimum_required(VERSION 3.5)
project(Funky-System C)
set(CMAKE_C_STANDARD 11)

include(ExternalProject)

set(VM_ARCH_BITS 32)
set(VM_NATIVE_MALLOC 0)

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
if (NOT MSVC)
        set_target_properties(libfunky-vm PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/${CMAKE_BUILD_TYPE}/libfunky-vm.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
else()
        set_target_properties(libfunky-vm PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/${CMAKE_BUILD_TYPE}/funky-vm.lib"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
endif()

include_directories("${source_dir}/include")


## FONTS
add_custom_target(
        fonts
        python3 ${CMAKE_CURRENT_SOURCE_DIR}/fonts/font_builder.py ${CMAKE_CURRENT_BINARY_DIR}
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


## RUNTIME
if(MSVC)
        add_custom_target(runtime cmd /C "${CMAKE_CURRENT_SOURCE_DIR}/runtime/build.cmd" "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
else()
        add_custom_target(runtime bash "${CMAKE_CURRENT_SOURCE_DIR}/runtime/build.sh" "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}" ${VM_ARCH_BITS})
endif()


## FUNKY
add_definitions(-DVM_ARCH_BITS=${VM_ARCH_BITS})
add_definitions(-DVM_NATIVE_MALLOC=${VM_NATIVE_MALLOC})
if (MSVC)
        set(SDL2_INCLUDE_DIRS "C:\\leeg\\SDL2-2.0.22\\include" CACHE PATH "SDL2 Include Dirs")
        set(SDL2_LIBRARIES "C:\\leeg\\SDL2-2.0.22\\lib\\x64\\SDL2main.lib" "C:\\leeg\\SDL2-2.0.22\\lib\\x64\\SDL2.lib" CACHE PATH "SDL2 Libraries")
else()
        INCLUDE(FindPkgConfig)
        PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
endif()
add_executable(funky main.c display.c display.h input.c input.h funky_system.h vm.c vm.h runtime/draw.c font.h runtime/input.c runtime/timer.c runtime/clipboard.c runtime/fs.c)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
set_property(SOURCE display.c APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/*.fnt.h)
add_dependencies(funky runtime fonts)
target_link_libraries(funky libfunky-vm ${SDL2_LIBRARIES})
target_link_directories(funky PRIVATE ${SDL2_LIBRARY_DIRS})
if (NOT MSVC)
        target_link_libraries(funky m)
endif()

if(APPLE)
    target_sources(funky PRIVATE os/macos/main.m)
    target_link_libraries(funky "-framework Foundation" "-framework Cocoa")
    configure_file(os/macos/Icon.png ${CMAKE_BINARY_DIR}/iconset/Icon.png COPYONLY)

    add_custom_target(icns bash ${CMAKE_CURRENT_SOURCE_DIR}/os/macos/create_icns.sh "${CMAKE_CURRENT_SOURCE_DIR}/os/macos/Icon.png" "${CMAKE_CURRENT_BINARY_DIR}/Funky System" )
    add_custom_target(bundle_base bash ${CMAKE_CURRENT_SOURCE_DIR}/os/macos/bundle.sh "Funky System" ${CMAKE_CURRENT_BINARY_DIR}/funky ${CMAKE_CURRENT_SOURCE_DIR}/os/macos/Info.plist "${CMAKE_CURRENT_BINARY_DIR}/Funky System.icns" )
    add_custom_target(bundle cp "${CMAKE_CURRENT_BINARY_DIR}/*.funk" "${CMAKE_CURRENT_BINARY_DIR}/Funky System.app/Contents/Resources/" )
    add_dependencies(bundle_base funky icns)
    add_dependencies(bundle bundle_base)
endif()

if (WIN32)
        target_sources(funky PRIVATE os/windows/win32.c)
        target_link_libraries(funky legacy_stdio_definitions.lib)
        #set_property(TARGET funky PROPERTY WIN32_EXECUTABLE true)
endif()
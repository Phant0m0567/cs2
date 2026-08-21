cmake_minimum_required(VERSION 3.20)

if(NOT WIN32)
    message(FATAL_ERROR "cs2-client is Windows-only (CS2 runs on Windows x64).")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "Build as x64. CS2 is a 64-bit process.")
endif()

project(cs2-client LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(MSVC)
    foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_BINARY_DIR}/bin")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} "${CMAKE_BINARY_DIR}/bin")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} "${CMAKE_BINARY_DIR}/bin")
    endforeach()
endif()

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.0
)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG master
)

FetchContent_MakeAvailable(imgui minhook)

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_compile_definitions(imgui_lib PUBLIC
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

add_library(minhook_lib STATIC
    ${minhook_SOURCE_DIR}/src/buffer.c
    ${minhook_SOURCE_DIR}/src/hook.c
    ${minhook_SOURCE_DIR}/src/trampoline.c
    ${minhook_SOURCE_DIR}/src/hde/hde64.c
)

target_include_directories(minhook_lib PUBLIC
    ${minhook_SOURCE_DIR}/include
)

add_library(cs2-client SHARED
    src/dllmain.cpp
    src/Entry.cpp
    src/Config.cpp
    src/Memory.cpp
    src/Renderer.cpp
    src/Menu.cpp
    src/Aim.cpp
    src/Game.cpp
)

target_include_directories(cs2-client PRIVATE include)

target_compile_definitions(cs2-client PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

target_link_libraries(cs2-client PRIVATE
    imgui_lib
    minhook_lib
    psapi
    d3d11
    dxgi
)

add_executable(cs2-client-loader
    src/Loader.cpp
)

target_compile_definitions(cs2-client-loader PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

target_include_directories(cs2-client-loader PRIVATE include)

if(MSVC)
    target_compile_options(cs2-client PRIVATE
        /W4
        /O2
        /fp:fast
    )
    target_compile_options(cs2-client-loader PRIVATE
        /W4
        /O2
        /fp:fast
    )

    target_link_options(cs2-client PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
    )
    target_link_options(cs2-client-loader PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
    )
endif()

set_target_properties(cs2-client PROPERTIES
    OUTPUT_NAME "cs2-client"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(cs2-client-loader PROPERTIES
    OUTPUT_NAME "cs2-client-loader"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

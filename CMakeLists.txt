cmake_minimum_required(VERSION 3.12)

# =========================================================
# Project setup
# =========================================================
project(USM-dev LANGUAGES C CXX)

include(ExternalProject)
include(cmake/i686-pc-mingw32.cmake OPTIONAL) # Safe if already provided on command line.

# =========================================================
# Options
# =========================================================
option(USE_GLM              "Enable glm third-party math library" OFF)
option(USE_FSTREAM          "Enable fstream usage in binkw32" OFF)
option(CHECK_STRUCT_SIZES   "Enable structure size checks" ON)
option(TARGET_XBOX          "Build Xbox specific sources" OFF)
option(ENABLE_TESTS         "Enable building tests (googletest)" ON)
option(BUILD_BINKW32_DLL    "Build binkw32 as a DLL" ON)
option(BUILD_BINKW32_EXE    "Build binkw32 as an executable (for WSL convenience)" ON)

# =========================================================
# Compiler / Flags
# =========================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Base warnings
set(PROJECT_WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wcast-align
    -Wcast-qual
    -Wzero-as-null-pointer-constant
    -Wctor-dtor-privacy
    -Wduplicated-branches
    -Wduplicated-cond
    -Wextra-semi
    -Wfloat-equal
    -Wlogical-op
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-promo
    -Wshift-negative-value
    -Wno-invalid-offsetof
    -Wno-deprecated
)

# Global compile options
add_compile_options(
    $<$<COMPILE_LANGUAGE:C>:-static>
    $<$<COMPILE_LANGUAGE:CXX>:-static>
    -g
)

# Additional C++ specific options (32-bit, etc.)
add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:CXX>:-m32>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-asynchronous-unwind-tables>
    $<$<COMPILE_LANGUAGE:CXX>:-masm=intel>
)

foreach(wflag IN LISTS PROJECT_WARNING_FLAGS)
    add_compile_options(${wflag})
endforeach()

# Force position independent code (shared libs, if any)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Linker flags (static, 32-bit)
set(CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS} -static -m32")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -m32")

if(MINGW)
    # Remove 'lib' prefix for imported Windows style names
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_SHARED_MODULE_PREFIX "")
endif()

# =========================================================
# Preprocessor definitions
# =========================================================
add_compile_definitions(CINTERFACE)
if(CHECK_STRUCT_SIZES)
    add_compile_definitions(CHECK_STRUCT_SIZES)
endif()
if(TARGET_XBOX)
    add_compile_definitions(TARGET_XBOX)
endif()
if(USE_GLM)
    add_compile_definitions(USE_GLM)
endif()

# =========================================================
# Third-party: glm (optional)
# =========================================================
if(USE_GLM)
    add_subdirectory(third_party/glm)
endif()

# =========================================================
# Utility & Core libs
# =========================================================
add_library(utility src/utility/trace.cpp)
target_include_directories(utility PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/utility>
)

add_library(variables src/variables/variables.cpp)
target_include_directories(variables PUBLIC "src/variables")
target_link_libraries(variables PUBLIC utility)

# =========================================================
# mash
# =========================================================
if (TARGET_XBOX)
    add_library(mash src/mash/xbox/src/mash_info_struct.cpp)
    target_include_directories(mash PUBLIC "src/mash/xbox/include/")
else()
    add_library(mash src/mash/pc/src/mash_info_struct.cpp)
    target_include_directories(mash PUBLIC "src/mash/pc/include")
endif()
target_include_directories(mash PUBLIC "src" "include")
target_link_libraries(mash PUBLIC utility variables)

# =========================================================
# memory / slab_allocator
# =========================================================
add_library(memory src/memory.cpp src/memory.h)
target_include_directories(memory PUBLIC "./include/")
target_link_libraries(memory PUBLIC utility)

add_library(slab_allocator src/slab_allocator.cpp src/slab_allocator.h)
target_include_directories(slab_allocator PUBLIC "./include/")
target_link_libraries(slab_allocator PUBLIC memory)

# =========================================================
# nalcomp / nalpanel / nsl / nal
# =========================================================
file(GLOB_RECURSE NALCOMP_SRC "./src/nalcomp/*.cpp")
add_library(nalcomp ${NALCOMP_SRC})
target_include_directories(nalcomp PUBLIC "./src/nalcomp/" "./src/")
target_link_libraries(nalcomp PUBLIC nal) # forward (nal defined later, so re-linked after nal creation if needed)

file(GLOB_RECURSE NALPANEL_SRC "./src/panels/nalpanel/*.cpp")
add_library(nalpanel ${NALPANEL_SRC})
target_include_directories(nalpanel PUBLIC "./src/panels/nalpanel/")
target_link_libraries(nalpanel PUBLIC nalcomp)

file(GLOB_RECURSE NSL_SRC "./src/nsl/src/nsl/*.cpp")
add_library(nsl ${NSL_SRC})
target_include_directories(nsl PUBLIC "./include/" "./src/" "./src/nsl/src/nsl/")
# nfl forward declared later

file(GLOB_RECURSE NAL_SRC "./src/nal_system.cpp" "./src/nal/source/common/*.cpp")
add_library(nal ${NAL_SRC})
target_include_directories(nal PUBLIC "./include/" "./src/" "./src/nal/include/common/")

# Temporary link ordering fixed after nfl definition.

# =========================================================
# chuck
# =========================================================
file(GLOB_RECURSE SRC_CHUCK "./src/chuck/vm/*.cpp")
add_library(chuck ${SRC_CHUCK})
target_include_directories(chuck
    PUBLIC "include" "src"
    INTERFACE "./src/chuck/vm/"
)
target_link_libraries(chuck PUBLIC slab_allocator variables)

# =========================================================
# tx
# =========================================================
file(GLOB_RECURSE SRC_TX "./src/tx/src/*.cpp" "./src/tx/src/*.h")
add_library(tx ${SRC_TX})
target_include_directories(tx PUBLIC "include" "src" INTERFACE "./src/tx/src/")
target_link_libraries(tx PUBLIC utility)

# =========================================================
# nfl
# =========================================================
file(GLOB_RECURSE SRC_NFL "./src/nfl/src/*.cpp" "./src/nfl/src/*.h")
add_library(nfl ${SRC_NFL})
target_include_directories(nfl PUBLIC "include" "src" INTERFACE "./src/nfl/src/")
target_link_libraries(nfl PUBLIC tx)

# Now finish linking previously forward libs
target_link_libraries(mash PUBLIC nfl)
target_link_libraries(nsl  PUBLIC nfl utility)
target_link_libraries(nal  PUBLIC nfl nalpanel utility)
target_link_libraries(nalcomp PUBLIC nal) # ensure nal after definition

# =========================================================
# ngl (dx + shaders + core)
# =========================================================
file(GLOB_RECURSE SRC_NGL_DX "./src/ngl/src/dx/*.cpp")
add_library(ngl_dx ${SRC_NGL_DX})
target_include_directories(ngl_dx PUBLIC "./src/ngl/include/dx" "./src/ngl/include/" "include" "src")
target_link_libraries(ngl_dx PUBLIC psapi utility variables)

file(GLOB_RECURSE SRC_NGL_SHADERS "./src/ngl/shaders/*.cpp")
add_library(ngl_shaders ${SRC_NGL_SHADERS})
target_include_directories(ngl_shaders PUBLIC "./src/ngl/shaders")
target_link_libraries(ngl_shaders PUBLIC ngl_dx)

file(GLOB SRC_NGL "./src/ngl/src/*.cpp")
add_library(ngl ${SRC_NGL})
target_include_directories(ngl PUBLIC "./src/ngl/include/")
target_link_libraries(ngl PUBLIC ngl_dx ngl_shaders)

# =========================================================
# charanimtype
# =========================================================
file(GLOB SRC_CHARANIMTYPE "./src/charanimtype/*.cpp")
add_library(charanimtype ${SRC_CHARANIMTYPE})
target_include_directories(charanimtype PUBLIC "./src/charanimtype/")
target_link_libraries(charanimtype PUBLIC nal ngl_dx ngl_shaders)

# =========================================================
# physics
# =========================================================
file(GLOB_RECURSE PHYSICS_SRC "./src/physics/source/*.cpp")
add_library(physics ${PHYSICS_SRC})
target_include_directories(physics PUBLIC "./src/physics/include/")
target_link_libraries(physics PUBLIC utility ngl)

# =========================================================
# Shared Sources for binkw32 (DLL/EXE)
# =========================================================
file(GLOB SRC_ROOT "./src/*.cpp" "./src/*.h")

# =========================================================
# binkw32 DLL (original)
# =========================================================
if(BUILD_BINKW32_DLL)
    add_library(binkw32 SHARED main.cpp ${SRC_ROOT})
    target_include_directories(binkw32 PUBLIC "include" "./src/nfl/src/" "./assimp/include/")
    if(USE_FSTREAM)
        target_compile_definitions(binkw32 PUBLIC USE_FSTREAM)
    endif()
endif()

# =========================================================
# binkw32 EXE (WSL convenience)
# =========================================================
if(BUILD_BINKW32_EXE)
    add_executable(binkw32_exe USM.cpp ${SRC_ROOT})
    target_include_directories(binkw32_exe PUBLIC "include" "./src/nfl/src/" "./assimp/include/")
    if(USE_FSTREAM)
        target_compile_definitions(binkw32_exe PUBLIC USE_FSTREAM)
    endif()
endif()

# =========================================================
# External: Assimp (static)
# =========================================================
ExternalProject_Add(
    AssimpProject
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/assimp
    BINARY_DIR  ${CMAKE_BINARY_DIR}/assimp-build
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/i686-pc-mingw32.cmake
        -DCMAKE_BUILD_TYPE=Release
        -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
        -DASSIMP_BUILD_TESTS=OFF
        -DASSIMP_NO_EXPORT=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DASSIMP_BUILD_ZLIB=ON
        -DASSIMP_BUILD_SAMPLES=OFF
        -DASSIMP_OPT_BUILD_PACKAGES=OFF
        -DCMAKE_CXX_FLAGS=-static-libgcc\ -static-libstdc++
    INSTALL_COMMAND ""
)

# Create an imported target after build
add_library(assimp::assimp_local STATIC IMPORTED GLOBAL)
add_dependencies(assimp::assimp_local AssimpProject)
set_target_properties(assimp::assimp_local PROPERTIES
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/assimp-build/lib/libassimp.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/assimp/include"
)

# =========================================================
# Link stacks for binkw32 targets
# =========================================================
set(_COMMON_LINK_LIBS
    physics
    charanimtype
    mash
    chuck
    nfl
    ngl
    nal
    nsl
    utility
    variables
    ngl_dx
    ngl_shaders
    nalpanel
    nalcomp
    slab_allocator
    memory
    tx
    assimp::assimp_local
)

if(MINGW)
    list(APPEND _COMMON_LINK_LIBS
        winmm
        psapi
        d3d9
        dinput8
        dxguid
        dsound
        d3dx9
        z
    )
endif()

if(USE_GLM)
    list(APPEND _COMMON_LINK_LIBS glm::glm)
endif()

if(BUILD_BINKW32_DLL)
    target_link_libraries(binkw32 PUBLIC ${_COMMON_LINK_LIBS})
endif()
if(BUILD_BINKW32_EXE)
    target_link_libraries(binkw32_exe PUBLIC ${_COMMON_LINK_LIBS})
endif()

# =========================================================
# Tests (optional)
# =========================================================
if(ENABLE_TESTS)
    set(GOOGLETEST_VERSION 1.1)
    add_subdirectory(third_party/googletest)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

# =========================================================
# Post-build copy (optional)
# =========================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Ultimate Spider-Man/")
    if(BUILD_BINKW32_DLL)
        add_custom_command(TARGET binkw32 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:binkw32>"
                "${CMAKE_CURRENT_SOURCE_DIR}/../Ultimate Spider-Man/binkw32.dll"
        )
    endif()
    if(BUILD_BINKW32_EXE)
        add_custom_command(TARGET binkw32_exe POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:binkw32_exe>"
                "${CMAKE_CURRENT_SOURCE_DIR}/../Ultimate Spider-Man/binkw32.exe"
        )
    endif()
endif()

# =========================================================
# Developer info
# =========================================================
message(STATUS "Configuration summary:")
message(STATUS "  TARGET_XBOX:        ${TARGET_XBOX}")
message(STATUS "  USE_GLM:            ${USE_GLM}")
message(STATUS "  USE_FSTREAM:        ${USE_FSTREAM}")
message(STATUS "  BUILD_BINKW32_DLL:  ${BUILD_BINKW32_DLL}")
message(STATUS "  BUILD_BINKW32_EXE:  ${BUILD_BINKW32_EXE}")
message(STATUS "  ENABLE_TESTS:       ${ENABLE_TESTS}")

cmake_minimum_required(VERSION 3.10)
project(qutemol)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VCG Library
set(VCG_PATH "" CACHE PATH "Path to vcglib")
if(NOT VCG_PATH)
    if(EXISTS "${CMAKE_SOURCE_DIR}/vcg")
        set(VCG_PATH "${CMAKE_SOURCE_DIR}/vcg")
    else()
        message(WARNING "VCG_PATH not set and 'vcg' directory not found in project root. VCG is required for building.")
    endif()
endif()

if(VCG_PATH)
    include_directories(${VCG_PATH})
endif()

# Dependencies
include_directories(src)

# Try to find wxWidgets, and install if missing
find_package(wxWidgets COMPONENTS core base gl)
if(NOT wxWidgets_FOUND)
    message(STATUS "wxWidgets not found. Attempting to install automatically...")
    if(WIN32)
        execute_process(COMMAND vcpkg install wxwidgets)
    elseif(APPLE)
        execute_process(COMMAND sudo port install wxWidgets-3.2 RESULT_VARIABLE macport_res)
        if(NOT macport_res EQUAL 0)
            execute_process(COMMAND brew install wxwidgets)
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo apt-get update)
            execute_process(COMMAND sudo apt-get install -y libwxgtk3.2-dev)
        elseif(DNF)
            execute_process(COMMAND sudo dnf install -y wxGTK3-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo pacman -S --noconfirm wxwidgets-gtk3)
        endif()
    endif()
    # Re-try finding wxWidgets
    find_package(wxWidgets COMPONENTS core base gl REQUIRED)
endif()
include(${wxWidgets_USE_FILE})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

# Try to find GIF, and install if missing
find_package(GIF)
if(NOT GIF_FOUND)
    message(STATUS "GIF library not found. Attempting to install automatically...")
    if(WIN32)
        execute_process(COMMAND vcpkg install giflib)
    elseif(APPLE)
        execute_process(COMMAND sudo port install giflib RESULT_VARIABLE macport_res)
        if(NOT macport_res EQUAL 0)
            execute_process(COMMAND brew install giflib)
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo apt-get update)
            execute_process(COMMAND sudo apt-get install -y libgif-dev)
        elseif(DNF)
            execute_process(COMMAND sudo dnf install -y giflib-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo pacman -S --noconfirm giflib)
        endif()
    endif()
    # Re-try finding GIF
    find_package(GIF REQUIRED)
endif()
include_directories(${GIF_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIRS})

# GIF symbol checks
include(CheckSymbolExists)
set(CMAKE_REQUIRED_INCLUDES ${GIF_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${GIF_LIBRARIES})

message(STATUS "GIF_INCLUDE_DIR: ${GIF_INCLUDE_DIR}")
message(STATUS "GIF_LIBRARIES: ${GIF_LIBRARIES}")

check_symbol_exists(GifQuantizeBuffer "gif_lib.h" HAS_GIF_QUANTIZE_BUFFER)
check_symbol_exists(QuantizeBuffer "gif_lib.h" HAS_QUANTIZE_BUFFER)
check_symbol_exists(GifMakeMapObject "gif_lib.h" HAS_GIF_MAKE_MAP_OBJECT)
check_symbol_exists(MakeMapObject "gif_lib.h" HAS_MAKE_MAP_OBJECT)

if(HAS_GIF_QUANTIZE_BUFFER)
    add_definitions(-DHAS_GIF_QUANTIZE_BUFFER)
endif()
if(HAS_QUANTIZE_BUFFER)
    add_definitions(-DHAS_QUANTIZE_BUFFER)
endif()
if(HAS_GIF_MAKE_MAP_OBJECT)
    add_definitions(-DHAS_GIF_MAKE_MAP_OBJECT)
endif()
if(HAS_MAKE_MAP_OBJECT)
    add_definitions(-DHAS_MAKE_MAP_OBJECT)
endif()

# Source files
set(SOURCES
    src/AO.cpp
    src/AOgpu.cpp
    src/AtomColor.cpp
    src/CgUtil.cpp
    src/Common.cpp
    src/HardSettings.cpp
    src/Mol.cpp
    src/MyCanvas.cpp
    src/MyTab.cpp
    src/Sampling.cpp
    src/ShadowMap.cpp
    src/gifSave.cpp
    src/main.cpp
    src/pngSave.cpp
    src/progress.cpp
    src/saveSnapDialog.cpp
    src/wx_trackball.cpp
)

# Headers
set(HEADERS
    src/AO.h
    src/AOgpu.h
    src/AtomColor.h
    src/CgUtil.h
    src/Common.h
    src/CubeMapSamp.h
    src/HardSettings.h
    src/Mol.h
    src/MyCanvas.h
    src/MyTab.h
    src/ShadowMap.h
    src/gen_normal.h
    src/gifSave.h
    src/main.h
    src/progress.h
    src/saveSnapDialog.h
    src/OctaMapSamp.h
)

if(WIN32)
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/main.rc")
        list(APPEND SOURCES src/main.rc)
    endif()
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_WINSOCKAPI_)
    # Enable WIN32 executable for WinMain
    set(GUI_TYPE WIN32)
endif()

if(VCG_PATH)
    if(EXISTS "${VCG_PATH}/wrap/gui/trackball.cpp")
        list(APPEND SOURCES "${VCG_PATH}/wrap/gui/trackball.cpp")
    elseif(EXISTS "${VCG_PATH}/vcg/wrap/gui/trackball.cpp")
        list(APPEND SOURCES "${VCG_PATH}/vcg/wrap/gui/trackball.cpp")
    endif()

    if(EXISTS "${VCG_PATH}/wrap/gui/trackmode.cpp")
        list(APPEND SOURCES "${VCG_PATH}/wrap/gui/trackmode.cpp")
    elseif(EXISTS "${VCG_PATH}/vcg/wrap/gui/trackmode.cpp")
        list(APPEND SOURCES "${VCG_PATH}/vcg/wrap/gui/trackmode.cpp")
    endif()
endif()

add_executable(qutemol ${GUI_TYPE} ${SOURCES} ${HEADERS})

target_link_libraries(qutemol
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${PNG_LIBRARIES}
    ${GIF_LIBRARIES}
    ${ZLIB_LIBRARIES}
    Eigen3::Eigen
)

# Installation
install(TARGETS qutemol DESTINATION bin)
install(DIRECTORY src/presets DESTINATION share/qutemol)
install(DIRECTORY src/image DESTINATION share/qutemol)

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
        find_program(VCPKG_EXE vcpkg)
        if(VCPKG_EXE)
            execute_process(COMMAND ${VCPKG_EXE} install wxwidgets)
        endif()
    elseif(APPLE)
        find_program(PORT_EXE port)
        find_program(BREW_EXE brew)
        if(PORT_EXE)
            # MacPorts: Try wxWidgets32
            execute_process(COMMAND sudo ${PORT_EXE} install wxWidgets32 RESULT_VARIABLE port_res)
            if(NOT port_res EQUAL 0)
                message(WARNING "MacPorts installation failed.")
            endif()
        elseif(BREW_EXE)
            execute_process(COMMAND ${BREW_EXE} install wxwidgets RESULT_VARIABLE brew_res)
            if(NOT brew_res EQUAL 0)
                message(WARNING "Homebrew installation failed.")
            endif()
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo ${APT_GET} update)
            execute_process(COMMAND sudo ${APT_GET} install -y libwxgtk3.2-dev)
        elseif(DNF)
            execute_process(COMMAND sudo ${DNF} install -y wxGTK3-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo ${PACMAN} -S --noconfirm wxwidgets-gtk3)
        endif()
    endif()

    # Re-try finding wxWidgets
    find_package(wxWidgets COMPONENTS core base gl)

    # Fallback to wx-config if find_package still fails on Unix-like systems
    if(NOT wxWidgets_FOUND AND NOT WIN32)
        find_program(WX_CONFIG_EXE wx-config)
        if(WX_CONFIG_EXE)
            message(STATUS "find_package failed, falling back to wx-config...")
            execute_process(COMMAND ${WX_CONFIG_EXE} --cppflags OUTPUT_VARIABLE WX_CPPFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND ${WX_CONFIG_EXE} --libs core,base,gl OUTPUT_VARIABLE WX_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

            # Extract include directories from cppflags (rough approximation)
            string(REGEX MATCHALL "-I([^ ]+)" WX_INCDIRS "${WX_CPPFLAGS}")
            foreach(inc ${WX_INCDIRS})
                string(SUBSTRING ${inc} 2 -1 clean_inc)
                list(APPEND wxWidgets_INCLUDE_DIRS ${clean_inc})
            endforeach()

            set(wxWidgets_LIBRARIES "${WX_LIBS}")
            set(wxWidgets_FOUND TRUE)
            message(STATUS "wxWidgets found via wx-config")
        endif()
    endif()

    if(NOT wxWidgets_FOUND)
        message(FATAL_ERROR "Could NOT find wxWidgets. Please install it manually.")
    endif()
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
        find_program(VCPKG_EXE vcpkg)
        if(VCPKG_EXE)
            execute_process(COMMAND ${VCPKG_EXE} install giflib)
        endif()
    elseif(APPLE)
        find_program(PORT_EXE port)
        find_program(BREW_EXE brew)
        if(PORT_EXE)
            execute_process(COMMAND sudo ${PORT_EXE} install giflib)
        elseif(BREW_EXE)
            execute_process(COMMAND ${BREW_EXE} install giflib)
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo ${APT_GET} update)
            execute_process(COMMAND sudo ${APT_GET} install -y libgif-dev)
        elseif(DNF)
            execute_process(COMMAND sudo ${DNF} install -y giflib-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo ${PACMAN} -S --noconfirm giflib)
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

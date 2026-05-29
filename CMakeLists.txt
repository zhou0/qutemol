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

# Advanced wxWidgets search: Look for wx-config in common paths and versions
if(NOT WIN32)
    find_program(WX_CONFIG_EXE
        NAMES wx-config wx-config-3.2 wx-config-3.1 wx-config-3.0
        PATHS /opt/local/bin /usr/local/bin /usr/bin /opt/homebrew/bin
    )
    if(WX_CONFIG_EXE)
        set(wxWidgets_CONFIG_EXECUTABLE ${WX_CONFIG_EXE})
        message(STATUS "Found wx-config: ${WX_CONFIG_EXE}")
    endif()
endif()

# Try to find wxWidgets
find_package(wxWidgets COMPONENTS core base gl)

# Auto-install logic if missing
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
            # Try wxWidgets-3.2 or 3.0
            execute_process(COMMAND sudo ${PORT_EXE} install wxWidgets-3.2 RESULT_VARIABLE port_res)
            if(port_res EQUAL 0)
                execute_process(COMMAND sudo ${PORT_EXE} select --set wxWidgets wxWidgets-3.2)
            else()
                execute_process(COMMAND sudo ${PORT_EXE} install wxWidgets-3.0 RESULT_VARIABLE port_res)
                if(port_res EQUAL 0)
                    execute_process(COMMAND sudo ${PORT_EXE} select --set wxWidgets wxWidgets-3.0)
                endif()
            endif()
        elseif(BREW_EXE)
            execute_process(COMMAND ${BREW_EXE} install wxwidgets)
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

    # Re-try detection after installation attempt
    if(NOT WIN32)
        find_program(WX_CONFIG_EXE_RETRY
            NAMES wx-config wx-config-3.2 wx-config-3.1 wx-config-3.0
            PATHS /opt/local/bin /usr/local/bin /usr/bin /opt/homebrew/bin
        )
        if(WX_CONFIG_EXE_RETRY)
            set(wxWidgets_CONFIG_EXECUTABLE ${WX_CONFIG_EXE_RETRY})
        endif()
    endif()

    find_package(wxWidgets COMPONENTS core base gl)

    # Manual fallback to wx-config if find_package still fails
    if(NOT wxWidgets_FOUND AND NOT WIN32)
        if(NOT wxWidgets_CONFIG_EXECUTABLE)
             find_program(wxWidgets_CONFIG_EXECUTABLE NAMES wx-config wx-config-3.2 wx-config-3.0)
        endif()

        if(wxWidgets_CONFIG_EXECUTABLE)
            message(STATUS "find_package failed, falling back to manual wx-config usage: ${wxWidgets_CONFIG_EXECUTABLE}")
            execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --cppflags OUTPUT_VARIABLE WX_CPPFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --libs core,base,gl OUTPUT_VARIABLE WX_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

            string(REGEX MATCHALL "-I([^ ]+)" WX_INCDIRS "${WX_CPPFLAGS}")
            foreach(inc ${WX_INCDIRS})
                string(SUBSTRING ${inc} 2 -1 clean_inc)
                list(APPEND wxWidgets_INCLUDE_DIRS ${clean_inc})
            endforeach()

            set(wxWidgets_LIBRARIES "${WX_LIBS}")
            set(wxWidgets_FOUND TRUE)
            message(STATUS "wxWidgets configured manually via wx-config")
        endif()
    endif()

    if(NOT wxWidgets_FOUND)
        message(FATAL_ERROR "Could NOT find wxWidgets. Please install it manually.")
    endif()
endif()
include(${wxWidgets_USE_FILE})

find_package(OpenGL)
if(NOT OPENGL_GLU_FOUND)
    message(STATUS "GLU not found. Attempting to install automatically...")
    if(WIN32)
        find_program(VCPKG_EXE vcpkg)
        if(VCPKG_EXE)
            execute_process(COMMAND ${VCPKG_EXE} install opengl)
        endif()
    elseif(APPLE)
        find_program(PORT_EXE port)
        find_program(BREW_EXE brew)
        if(PORT_EXE)
            execute_process(COMMAND sudo ${PORT_EXE} install libGLU)
        elseif(BREW_EXE)
            execute_process(COMMAND ${BREW_EXE} install freeglut)
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo ${APT_GET} update)
            execute_process(COMMAND sudo ${APT_GET} install -y libglu1-mesa-dev)
        elseif(DNF)
            execute_process(COMMAND sudo ${DNF} install -y mesa-libGLU-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo ${PACMAN} -S --noconfirm glu)
        endif()
    endif()
    find_package(OpenGL REQUIRED)
endif()
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW)
if(NOT GLEW_FOUND)
    message(STATUS "GLEW not found. Attempting to install automatically...")
    if(WIN32)
        find_program(VCPKG_EXE vcpkg)
        if(VCPKG_EXE)
            execute_process(COMMAND ${VCPKG_EXE} install glew)
        endif()
    elseif(APPLE)
        find_program(PORT_EXE port)
        find_program(BREW_EXE brew)
        if(PORT_EXE)
            execute_process(COMMAND sudo ${PORT_EXE} install glew)
        elseif(BREW_EXE)
            execute_process(COMMAND ${BREW_EXE} install glew)
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo ${APT_GET} update)
            execute_process(COMMAND sudo ${APT_GET} install -y libglew-dev)
        elseif(DNF)
            execute_process(COMMAND sudo ${DNF} install -y glew-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo ${PACMAN} -S --noconfirm glew)
        endif()
    endif()
    find_package(GLEW REQUIRED)
endif()
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

find_package(Eigen3)
if(NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found. Attempting to install automatically...")
    if(WIN32)
        find_program(VCPKG_EXE vcpkg)
        if(VCPKG_EXE)
            execute_process(COMMAND ${VCPKG_EXE} install eigen3)
        endif()
    elseif(APPLE)
        find_program(PORT_EXE port)
        find_program(BREW_EXE brew)
        if(PORT_EXE)
            execute_process(COMMAND sudo ${PORT_EXE} install eigen3)
        elseif(BREW_EXE)
            execute_process(COMMAND ${BREW_EXE} install eigen)
        endif()
    elseif(UNIX)
        find_program(APT_GET apt-get)
        find_program(DNF dnf)
        find_program(PACMAN pacman)
        if(APT_GET)
            execute_process(COMMAND sudo ${APT_GET} update)
            execute_process(COMMAND sudo ${APT_GET} install -y libeigen3-dev)
        elseif(DNF)
            execute_process(COMMAND sudo ${DNF} install -y eigen3-devel)
        elseif(PACMAN)
            execute_process(COMMAND sudo ${PACMAN} -S --noconfirm eigen3)
        endif()
    endif()
    find_package(Eigen3 REQUIRED)
endif()
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
    src/qopengl.h
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

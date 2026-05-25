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

find_package(wxWidgets COMPONENTS core base gl REQUIRED)
include(${wxWidgets_USE_FILE})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

find_package(GIF REQUIRED)
include_directories(${GIF_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

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
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
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

add_executable(qutemol ${SOURCES} ${HEADERS})

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

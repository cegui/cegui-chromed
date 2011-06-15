cmake_minimum_required(VERSION 2.8)
if (POLICY CMP0017)
    cmake_policy(SET CMP0017 OLD)
endif()

set(CHROMED_CEGUI_INCLUDE_DIR "include")
set(CHROMED_CEGUI_SRC_DIR "src")

# FIXME: nasty
set(CEGUI_INCLUDE_PATH "/home/kulik/Devel/CEGUI/cegui_mk2/cegui/include" "/home/kulik/Devel/CEGUI/cegui_mk2/build/cegui/include")
set(BERKELIUM_INCLUDE_PATH "/home/kulik/Devel/CEGUI/ChromedCEGUI/berkelium/include")

project(ChromedCEGUI)

file (GLOB CHROMED_CEGUI_SOURCE_FILES ${CHROMED_CEGUI_SRC_DIR}/*.cpp)
include_directories(${CHROMED_CEGUI_INCLUDE_DIR} ${CEGUI_INCLUDE_PATH} ${BERKELIUM_INCLUDE_PATH})
add_library(ChromedCEGUI SHARED ${CHROMED_CEGUI_SOURCE_FILES})

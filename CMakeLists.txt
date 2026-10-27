cmake_minimum_required(VERSION 3.21)
project(Crosimage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "CMake")
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Svg Xml)

option(CROSIMAGE_DEPLOY_QT "Copy Qt runtime libraries and plugins beside the Windows executable" ${WIN32})
add_subdirectory(Crosimage)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Crosimage)

# Make the former solution items available in generated IDE projects as well.
add_custom_target(SolutionItems SOURCES README.md AGENTS.md build.py todo.txt)
set_target_properties(SolutionItems PROPERTIES PROJECT_LABEL "Solution Items")

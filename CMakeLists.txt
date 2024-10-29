cmake_minimum_required(VERSION 3.24)
project(neat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Configuration-specific compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Svg SvgWidgets Xml)

# Source files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/image_presenter.cpp
    ${CMAKE_SOURCE_DIR}/src/custom_graphics_view.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    # ${CMAKE_SOURCE_DIR}/src/input_device_detector.cpp
)

# Header files
set(HEADERS
    ${CMAKE_SOURCE_DIR}/include/image_presenter.h
    ${CMAKE_SOURCE_DIR}/include/custom_graphics_view.h
    ${CMAKE_SOURCE_DIR}/include/utils.h
    ${CMAKE_SOURCE_DIR}/include/json.hpp
    # ${CMAKE_SOURCE_DIR}/include/input_device_detector.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link Qt libraries and other dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Xml
    stdc++fs
)

qt_standard_project_setup()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy icon file to build directory
configure_file(${CMAKE_SOURCE_DIR}/resources/icon.png ${CMAKE_BINARY_DIR}/icon.png COPYONLY)

# Install icon file
install(FILES ${CMAKE_SOURCE_DIR}/resources/icon.png
    DESTINATION share/${PROJECT_NAME}
)

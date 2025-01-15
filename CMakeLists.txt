cmake_minimum_required(VERSION 3.24)
project(neat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# RPATH settings for Flatpak
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/lib:$ORIGIN/../../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Make sure plugins can find Qt libraries
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

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

qt_standard_project_setup()

# Add executable
qt_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

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
)

# Install target
install(TARGETS ${PROJECT_NAME}
BUNDLE  DESTINATION .
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install icon files
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-512x512.png
    DESTINATION share/icons/hicolor/512x512/apps
    RENAME com.acrilique.neat.png
)

# Install downscaled icons
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-16x16.png
    DESTINATION share/icons/hicolor/16x16/apps
    RENAME com.acrilique.neat.png
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-24x24.png
    DESTINATION share/icons/hicolor/24x24/apps
    RENAME com.acrilique.neat.png
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-32x32.png
    DESTINATION share/icons/hicolor/32x32/apps
    RENAME com.acrilique.neat.png
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-48x48.png
    DESTINATION share/icons/hicolor/48x48/apps
    RENAME com.acrilique.neat.png
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-64x64.png
    DESTINATION share/icons/hicolor/64x64/apps
    RENAME com.acrilique.neat.png
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-128x128.png
    DESTINATION share/icons/hicolor/128x128/apps
    RENAME com.acrilique.neat.png
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat-256x256.png
    DESTINATION share/icons/hicolor/256x256/apps
    RENAME com.acrilique.neat.png
)

# Install desktop file
install(FILES ${CMAKE_SOURCE_DIR}/resources/com.acrilique.neat.desktop
    DESTINATION share/applications
)

# Let Flatpak handle Qt plugin deployment

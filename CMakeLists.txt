cmake_minimum_required(VERSION 3.5)

project (qrcoder)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


if (MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions( -DHAVE_QT)

# Find the QtWidgets library
find_package(Qt5 CONFIG REQUIRED Widgets)
find_package(Qt5PrintSupport REQUIRED)

# Find the QREncoder library
find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h)
find_library(QRENCODE_LIBRARY_RELEASE qrencode)
find_library(QRENCODE_LIBRARY_DEBUG qrencoded)
set(QRENCODE_LIBRARIES optimized ${QRENCODE_LIBRARY_RELEASE} debug ${QRENCODE_LIBRARY_DEBUG})

set(project_SOURCES main.cpp QRCoder.cpp QRCoder.ui)

add_executable(${PROJECT_NAME} ${project_SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport ${QRENCODE_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${QRENCODE_INCLUDE_DIR})
qt5_use_modules(${PROJECT_NAME} Widgets)

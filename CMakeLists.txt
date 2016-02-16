cmake_minimum_required(VERSION 3.0.0)
set(project_name "ImageCLEF")

# set the project name
project("${project_name}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
SET(CMAKE_PREFIX_PATH : "/usr/local/Cellar/qt5/5.5.1_2")

# OpenCV package
FIND_PACKAGE(OpenCV 3 REQUIRED)

# Qt5 package
FIND_PACKAGE(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(HEADERS include/mainwindow.h include/Reader.h)
set(SOURCES main.cpp src/mainwindow.cpp src/Reader.cpp)

# Create Qt MOC files
QT5_WRAP_UI(UISrcs forms/mainwindow.ui)

add_executable("${project_name}" ${SOURCES} ${HEADERS} ${UISrcs})
target_link_libraries( "${project_name}" ${OpenCV_LIBS} ${QT_LIBRARIES})

qt5_use_modules("${project_name}" Core Gui Widgets)

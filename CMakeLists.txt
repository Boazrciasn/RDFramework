cmake_minimum_required(VERSION 3.0.0)
set(project_name "ImageCLEF")

# set the project name
project("${project_name}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
SET(CMAKE_PREFIX_PATH : "/usr/local/Cellar/qt5/5.5.1_2")

# OpenCV package
FIND_PACKAGE(OpenCV 3 REQUIRED)

#add OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS} include/)

# Qt5 package
FIND_PACKAGE(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)

# NM Not tested yet
find_package(Qt5PrintSupport REQUIRED)

set(QT_USE_QTXML TRUE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


file(GLOB SOURCES main.cpp src/* qcustomplot/qcustomplot.cpp)
file(GLOB HEADERS include/* qcustomplot/qcustomplot.h)

# Create Qt MOC files
QT5_WRAP_UI(UISrcs forms/mainwindow.ui forms/mydialog.ui forms/annextractordialog.ui forms/dialogextracted.ui forms/histogramdialog.ui)

add_executable("${project_name}" ${SOURCES} ${HEADERS} ${UISrcs})

# NM Not tested yet
target_link_libraries( "${project_name}" ${OpenCV_LIBS} ${QT_LIBRARIES} Qt5::Xml Qt5::PrintSupport)

qt5_use_modules("${project_name}" Core Gui Widgets)

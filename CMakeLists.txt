cmake_minimum_required(VERSION 3.0.0)
set(project_name "ImageCLEF")

# set the project name
project("${project_name}")

SET(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fopenmp")

# OpenCV package
FIND_PACKAGE(OpenCV 3 REQUIRED)

#add OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS}  include/gui/
                                            include/ocr/ include/rdf/
                                            include/tracking/ include/util/)

INCLUDE_DIRECTORIES(qcustomplot/)

# Qt5 package
FIND_PACKAGE(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

# NM Not tested yet
find_package(Qt5PrintSupport REQUIRED)

set(QT_USE_QTXML TRUE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#SOURCE FILES
file(GLOB SOURCES main.cpp src/*.cpp  qcustomplot/qcustomplot.cpp cereal/*)
file(GLOB GUISOURCES src/gui/*.cpp)
file(GLOB RDFSOURCES src/rdf/*.cpp)
file(GLOB OCRSOURCES src/ocr/*.cpp)
file(GLOB TRACKSOURCES src/tracking/*.cpp)
file(GLOB PFSOURCES src/tracking/particlefilter/*.cpp)
file(GLOB UTILSOURCES src/util/*.cpp src/util/*.h)

#HEADER FILES
file(GLOB HEADERS include/*.h qcustomplot/qcustomplot.h cereal/*)
file(GLOB GUIHEADERS include/gui/*.h)
file(GLOB RDFHEADERS include/rdf/*.h)
file(GLOB OCRHEADERS include/ocr/*.h)
file(GLOB TRACKHEADERS include/tracking/*.h)
file(GLOB PFHEADERS include/tracking/particlefilter/*.h)
file(GLOB UTILHEADERS include/util/*.h include/util/*.hpp)

# Create Qt MOC files
QT5_WRAP_UI(UISrcs forms/MainWindow.ui forms/MyDialog.ui forms/AnnExtractorDialog.ui
                   forms/ExtractedDialog.ui forms/HistogramDialog.ui forms/RDFDialog.ui
                   forms/ResizeallImagesDialog.ui forms/DisplayImagesWidget.ui forms/PFWidget.ui)

add_executable("${project_name}" ${SOURCES} ${GUISOURCES} ${RDFSOURCES} ${OCRSOURCES} ${TRACKSOURCES}  ${PFSOURCES}
                ${UTILSOURCES} ${GUIHEADERS} ${RDFHEADERS} ${OCRHEADERS} ${TRACKHEADERS} ${PFHEADERS} ${UTILHEADERS} ${HEADERS} ${UISrcs})

# NM Not tested yet
target_link_libraries( "${project_name}" ${OpenCV_LIBS} ${QT_LIBRARIES} Qt5::Xml Qt5::PrintSupport Qt5::Multimedia Qt5::MultimediaWidgets)

qt5_use_modules("${project_name}" Core Gui Widgets)


################################# TBB #################################
#IF (APPLE)
#    SET(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/Users/furkankirac/DEV/tbb44_20160413oss/")
#ENDIF ()
## FIND_PACKAGE(TBB REQUIRED)
#INCLUDE(DetectTBB REQUIRED)
#IF (APPLE)
#    ADD_AS_LIB(TBB "libtbb.dylib")
#ENDIF ()

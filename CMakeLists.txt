cmake_minimum_required(VERSION 3.0.0)
set(project_name "ImageCLEF")

# set the project name
project("${project_name}")

SET(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fopenmp")

# OpenCV package
FIND_PACKAGE(OpenCV 3 REQUIRED)

#add OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS}  Core/       3rdparty/qcustomplot/ 3rdparty/cereal/
                                            modules/    util/)

# Qt5 package
FIND_PACKAGE(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

# TBB package
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/")
find_package(TBB COMPONENTS tbbmalloc tbbmalloc_proxy tbb_preview)

# NM Not tested yet
find_package(Qt5PrintSupport REQUIRED)

set(QT_USE_QTXML TRUE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


file(GLOB CORE Core/*.ui Core/*.cpp Core/*.h Core/*.hpp)
file(GLOB OCR modules/ocr/*.ui modules/ocr/*.cpp modules/ocr/*.h modules/ocr/*.hpp)
file(GLOB RDF modules/rdf/*.ui modules/rdf/*.cpp modules/rdf/*.h modules/rdf/*.hpp)
file(GLOB PF modules/tracking/particlefilter/*.ui modules/tracking/particlefilter/*.cpp
             modules/tracking/particlefilter/*.h  modules/tracking/particlefilter/*.hpp)
file(GLOB VIDEOPLAYER modules/tracking/videoplayer/*.ui modules/tracking/videoplayer/*.cpp
                      modules/tracking/videoplayer/*.h modules/tracking/videoplayer/*.hpp)
file(GLOB UTIL util/*.ui util/*.cpp util/*.h util/*.hpp)
file(GLOB QCUSTOMPLOT 3rdparty/qcustomplot/*.ui 3rdparty/qcustomplot/*.cpp
                      3rdparty/qcustomplot/*.h 3rdparty/qcustomplot/*.hpp)
file(GLOB CEREAL 3rdparty/cereal/*)
file(GLOB 3RDPARTY 3rdparty/*.ui 3rdparty/*.cpp 3rdparty/*.h 3rdparty/*.hpp)
file(GLOB DEXTRACT modules/tracking/dataExtraction/*.cpp modules/tracking/dataExtraction/*.hpp
                   modules/tracking/dataExtraction/*.h modules/tracking/dataExtraction/*.ui)



# Create Qt MOC files
QT5_WRAP_UI(UISrcs Core/MainWindowGui.ui Core/DisplayImagesWidgetGui.ui Core/ResizeAllImagesDialogGui.ui
                    modules/ocr/AnnExtractorDialogGui.ui modules/ocr/ExtractedDialogGui.ui modules/ocr/HistogramDialogGui.ui modules/ocr/MyDialogGui.ui
                    modules/rdf/RandomDecisionForestDialogGui.ui
                    modules/tracking/particlefilter/ParticleFilterWidgetGui.ui
                    modules/tracking/dataExtraction/DataExtractorGui.ui
                    modules/tracking/dataExtraction/PredictorGui.ui
                    modules/tracking/videoplayer/VideoPlayerGui.ui)

add_executable("${project_name}" ${CORE} ${OCR} ${RDF} ${PF} ${VIDEOPLAYER}  ${UTIL}
                ${UTIL} ${QCUSTOMPLOT} ${CEREAL} ${3RDPARTY} ${DEXTRACT} ${UISrcs})

# NM Not tested yet
target_link_libraries( "${project_name}" ${OpenCV_LIBS} ${QT_LIBRARIES} tbb Qt5::Xml Qt5::PrintSupport Qt5::Multimedia Qt5::MultimediaWidgets)

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

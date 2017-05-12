cmake_minimum_required(VERSION 3.2)
set(project_name "RDF")

# set the project name
project("${project_name}")
SET(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -fopenmp -O3")

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/")

# Find packages
FIND_PACKAGE(OpenCV 3 REQUIRED)
FIND_PACKAGE(CUDA REQUIRED)
FIND_PACKAGE(Boost)
FIND_PACKAGE(TBB COMPONENTS tbbmalloc tbbmalloc_proxy tbb_preview)

# Qt5 package
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)
FIND_PACKAGE(Qt5Multimedia REQUIRED)
FIND_PACKAGE(Qt5MultimediaWidgets REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)

INCLUDE(FindCUDA)
SET(CUDA_INCLUDE_DIRS "/usr/local/cuda/include")
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    ${CUDA_INCLUDE_DIRS}
                    ${TBB_INCLUDE_DIRS}
                    Core/       3rdparty/qcustomplot/ 3rdparty/cereal/
                                            modules/    util/)

SET(QT_USE_QTXML TRUE)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)


FILE(GLOB CORE Core/*.ui Core/*.cpp Core/*.h Core/*.hpp)
FILE(GLOB OCR modules/ocr/*.ui modules/ocr/*.cpp modules/ocr/*.h modules/ocr/*.hpp)
FILE(GLOB RDF modules/rdf/*.ui modules/rdf/*.cpp modules/rdf/*.h modules/rdf/*.hpp)
FILE(GLOB PF modules/tracking/particlefilter/*.ui modules/tracking/particlefilter/*.cpp
    modules/tracking/particlefilter/*.h  modules/tracking/particlefilter/*.hpp)
FILE(GLOB VIDEOPLAYER modules/tracking/videoplayer/*.ui modules/tracking/videoplayer/*.cpp
    modules/tracking/videoplayer/*.h modules/tracking/videoplayer/*.hpp)
FILE(GLOB UTIL util/*.ui util/*.cpp util/*.h util/*.hpp)
FILE(GLOB QCUSTOMPLOT 3rdparty/qcustomplot/*.ui 3rdparty/qcustomplot/*.cpp
    3rdparty/qcustomplot/*.h 3rdparty/qcustomplot/*.hpp)
FILE(GLOB CEREAL 3rdparty/cereal/*)
FILE(GLOB 3RDPARTY 3rdparty/*.ui 3rdparty/*.cpp 3rdparty/*.h 3rdparty/*.hpp)
FILE(GLOB PCG 3rdparty/pcg-cpp-0.98/include/*.hpp)
FILE(GLOB DEXTRACT modules/tracking/dataExtraction/*.cpp modules/tracking/dataExtraction/*.hpp
    modules/tracking/dataExtraction/*.h modules/tracking/dataExtraction/*.ui)

# CUDA
FILE(GLOB CUDA_FILES modules/rdf/*.cu)

# Create Qt MOC files
QT5_WRAP_UI(UISrcs  Core/MainWindowGui.ui Core/DisplayImagesWidgetGui.ui Core/ResizeAllImagesDialogGui.ui Core/DisplayGUI.ui Core/PreProcessGUI.ui
    modules/ocr/AnnExtractorDialogGui.ui modules/ocr/ExtractedDialogGui.ui modules/ocr/HistogramDialogGui.ui modules/ocr/MyDialogGui.ui
    modules/rdf/RandomDecisionForestDialogGui.ui
    modules/tracking/particlefilter/ParticleFilterWidgetGui.ui
    modules/tracking/dataExtraction/DataExtractorGui.ui
    modules/tracking/dataExtraction/PredictorGui.ui
    modules/tracking/videoplayer/VideoPlayerGui.ui
    util/ReaderGUI.ui)


if (CUDA_FOUND)
    LIST(APPEND CMAKE_CXX_FLAGS "-ffast-math")
    LIST(APPEND CUDA_NVCC_FLAGS --compiler-options -fno-strict-aliasing -lineinfo -use_fast_math -Xptxas -dlcm=cg)
    LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_50,code=sm_50)

    CUDA_ADD_EXECUTABLE("${project_name}" ${CORE} ${OCR} ${RDF} ${PF} ${VIDEOPLAYER}  ${UTIL}
        ${UTIL} ${QCUSTOMPLOT} ${CEREAL} ${PCG} ${3RDPARTY} ${DEXTRACT} ${UISrcs} ${CUDA_FILES})

    target_link_libraries( "${project_name}" ${OpenCV_LIBS} ${Boost_LIBRARIES} ${QT_LIBRARIES}
        tbb Qt5::Xml Qt5::PrintSupport Qt5::Multimedia Qt5::MultimediaWidgets /usr/local/cuda/lib64/libcudart.so)
else()

    add_executable("${project_name}" ${CORE} ${OCR} ${RDF} ${PF} ${VIDEOPLAYER}  ${UTIL}
        ${UTIL} ${QCUSTOMPLOT} ${CEREAL} ${PCG} ${3RDPARTY} ${DEXTRACT} ${UISrcs})

    target_link_libraries( "${project_name}" ${OpenCV_LIBS} ${Boost_LIBRARIES} ${QT_LIBRARIES}
        tbb Qt5::Xml Qt5::PrintSupport Qt5::Multimedia Qt5::MultimediaWidgets)

endif()

qt5_use_modules("${project_name}" Core Gui Widgets)

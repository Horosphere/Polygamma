# Pre-requisites:
# Python3, Boost 1.61.0, avcodec, avformat, avutil, swscale, Qt 5.6
project(Polygamma)
cmake_minimum_required(VERSION 3.5)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


# Enable C++ 14
add_compile_options(-std=c++14)

# Include Python
set(Python_ADDITIONAL_VERSIONS 3.5)
find_package(PythonLibs)
if (${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
	link_directories(${PYTHON_LIBRARIES})
endif()

# Boost
find_package(Boost 1.61.0 COMPONENTS thread timer python3 REQUIRED)
if (${Boost_FOUND})
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIR})
endif()

# Qt
find_package(Qt5Widgets REQUIRED)
#add_definitions(-DQT_CONSOLE)
add_definitions(-DQT_NO_KEYWORDS)

qt5_add_resources(QtCompiledResources resources/Polygamma.qrc)

# Do not remove the following auto-generated signatures

# Auto-generated. Do not edit. All changes will be undone
set(SourceFiles
    src/main.cpp
    src/singular/BufferSingular.cpp
    src/core/Kernel.cpp
    src/core/python.cpp
    src/core/Configuration.cpp
    src/core/Buffer.cpp
    src/math/fourier.cpp
    src/ui/mainWindowAccessories.cpp
    src/ui/MainWindow.cpp
    src/ui/ui.cpp
    src/ui/Terminal.cpp
    src/ui/DialogPreferences.cpp
    src/ui/util/SyntaxHighlighterPython.cpp
    src/ui/util/ColourButton.cpp
    src/ui/util/Axis.cpp
    src/ui/editors/EditorSingular.cpp
    src/ui/editors/Editor.cpp
    src/ui/graphics/Viewport2.cpp
    src/ui/graphics/Waveform.cpp
    src/ui/panels/Panel.cpp
   )
set(QObjectHeaders
    src/ui/DialogPreferences.hpp
    src/ui/ui.hpp
    src/ui/mainWindowAccessories.hpp
    src/ui/MainWindow.hpp
    src/ui/Terminal.hpp
    src/ui/util/SyntaxHighlighterPython.hpp
    src/ui/util/Axis.hpp
    src/ui/util/ColourButton.hpp
    src/ui/editors/Editor.hpp
    src/ui/editors/EditorSingular.hpp
    src/ui/graphics/Viewport2.hpp
    src/ui/graphics/Waveform.hpp
    src/ui/panels/Panel.hpp
   )
# Auto-generated end

qt5_wrap_cpp(QtMOCSourceFiles ${QObjectHeaders})

add_executable(Polygamma
	${SourceFiles}
	${QtMOCSourceFiles}
	${QtCompiledResources})
target_link_libraries(Polygamma ${PYTHON_LIBRARIES})
target_link_libraries(Polygamma ${Boost_LIBRARIES})
target_link_libraries(Polygamma Qt5::Widgets Qt5::Core)
target_link_libraries(Polygamma avcodec avformat avutil swscale)


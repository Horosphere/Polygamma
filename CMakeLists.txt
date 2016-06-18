# Pre-requisites:
# Qt5.6, Boost 1.61.0, avcodec, avformat, avutil, swscale
project(Polygamma)
cmake_minimum_required(VERSION 3.5)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(QtResources resources/Polygamma.qrc)

# Enable C++ 11
add_compile_options(-std=c++11)

# Boost
find_package(Boost 1.61.0 COMPONENTS thread REQUIRED)
if (${Boost_FOUND})
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIR})
endif()

# Qt
find_package(Qt5Widgets REQUIRED)
add_definitions(-DQT_CONSOLE)
add_definitions(-DQT_NO_KEYWORDS)

# Do not remove the following auto-generated signatures

# Auto-generated. Do not edit. All changes will be undone
set(SourceFiles
    src/main.cpp
    src/core/Kernel.cpp
    src/core/Configuration.cpp
    src/io/av.cpp
    src/math/Audio.cpp
    src/math/fourier.cpp
    src/ui/moc_MainWindow.cpp
    src/ui/moc_DialogPreferences.cpp
    src/ui/MainWindow.cpp
    src/ui/ui.cpp
    src/ui/Terminal.cpp
    src/ui/moc_ui.cpp
    src/ui/moc_Terminal.cpp
    src/ui/DialogPreferences.cpp
    src/ui/util/SyntaxHighlighterPython.cpp
    src/ui/util/moc_ColourButton.cpp
    src/ui/util/moc_SyntaxHighlighterPython.cpp
    src/ui/util/ColourButton.cpp
    src/ui/editors/moc_EditorSimple.cpp
    src/ui/editors/EditorSimple.cpp
    src/ui/editors/Editor.cpp
    src/ui/editors/moc_Editor.cpp
    src/ui/graphics/TickedAxis.cpp
    src/ui/graphics/moc_TickedAxis.cpp
    src/ui/graphics/moc_Waveform.cpp
    src/ui/graphics/Spectrogram.cpp
    src/ui/graphics/moc_Spectrogram.cpp
    src/ui/graphics/Viewport2.cpp
    src/ui/graphics/Waveform.cpp
    src/ui/graphics/moc_Viewport2.cpp
    src/ui/panels/moc_PanelBase.cpp
    src/ui/panels/PanelBase.cpp
   )
set(QObjectHeaders
    src/ui/DialogPreferences.hpp
    src/ui/ui.hpp
    src/ui/MainWindow.hpp
    src/ui/Terminal.hpp
    src/ui/util/SyntaxHighlighterPython.hpp
    src/ui/util/ColourButton.hpp
    src/ui/editors/Editor.hpp
    src/ui/editors/EditorSimple.hpp
    src/ui/graphics/Viewport2.hpp
    src/ui/graphics/Waveform.hpp
    src/ui/graphics/TickedAxis.hpp
    src/ui/graphics/Spectrogram.hpp
    src/ui/panels/PanelBase.hpp
   )
# Auto-generated end

qt5_wrap_cpp(QtMOCSourceFiles ${QObjectHeaders})
qt5_add_resources(QtCompiledResources ${QtResources})

add_executable(Polygamma
	${SourceFiles}
	${QtMOCSourceFiles}
	${QtCompiledResources})
target_link_libraries(Polygamma Qt5::Widgets Qt5::Core)
target_link_libraries(Polygamma avcodec avformat avutil swscale)
target_link_libraries(Polygamma ${Boost_LIBRARIES})

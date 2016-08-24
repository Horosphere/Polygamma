project(Polygamma)
# Pre-requisites:
# Python3, Boost 1.61.0, avcodec, avformat, avutil, swscale, SDL2, Qt 5
cmake_minimum_required(VERSION 3.5)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(StdFeatures
	cxx_range_for
	cxx_noexcept
	cxx_rvalue_references
	cxx_override
	cxx_lambdas
	cxx_final
	cxx_deleted_functions
	cxx_delegating_constructors
	cxx_auto_type
	cxx_constexpr
	)
# Include Python
set(Python_ADDITIONAL_VERSIONS 3.5)
find_package(PythonLibs 3.5 REQUIRED)
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

# Threads
find_package(Threads REQUIRED)

qt5_add_resources(QtCompiledResources resources/Polygamma.qrc)

# Do not remove the following auto-generated signatures

# Auto-generated. Do not edit. All changes will be undone
set(SourceFiles
    src/main.cpp
    src/core/Buffer.cpp
    src/core/Configuration.cpp
    src/core/Kernel.cpp
    src/core/python.cpp
    src/ui/Terminal.cpp
    src/ui/mainWindowAccessories.cpp
    src/ui/MainWindow.cpp
    src/ui/ui.cpp
    src/ui/graphics/Viewport2.cpp
    src/ui/graphics/Cursor.cpp
    src/ui/graphics/Waveform.cpp
    src/ui/editors/EditorSingular.cpp
    src/ui/editors/Editor.cpp
    src/ui/panels/Panel.cpp
    src/ui/panels/PanelPlayback.cpp
    src/ui/util/Axis.cpp
    src/ui/util/SyntaxHighlighterPython.cpp
    src/ui/util/actions.cpp
    src/ui/util/ColourButton.cpp
    src/ui/dialogs/DialogNewSingular.cpp
    src/ui/dialogs/DialogScriptArgs.cpp
    src/ui/dialogs/DialogPreferences.cpp
    src/singular/BufferSingular.cpp
    src/singular/audio.cpp
    src/math/fourier.cpp
    src/media/media.c
    src/media/playback.c
    src/media/io.c
   )
set(QObjectHeaders
    src/ui/mainWindowAccessories.hpp
    src/ui/Terminal.hpp
    src/ui/MainWindow.hpp
    src/ui/ui.hpp
    src/ui/graphics/Waveform.hpp
    src/ui/graphics/Viewport2.hpp
    src/ui/graphics/Cursor.hpp
    src/ui/editors/EditorSingular.hpp
    src/ui/editors/Editor.hpp
    src/ui/panels/Panel.hpp
    src/ui/panels/PanelPlayback.hpp
    src/ui/util/ColourButton.hpp
    src/ui/util/SyntaxHighlighterPython.hpp
    src/ui/util/actions.hpp
    src/ui/util/Axis.hpp
    src/ui/dialogs/DialogScriptArgs.hpp
    src/ui/dialogs/DialogNewSingular.hpp
    src/ui/dialogs/DialogPreferences.hpp
   )
# Auto-generated end

qt5_wrap_cpp(QtMOCSourceFiles ${QObjectHeaders})

add_executable(Polygamma
	${SourceFiles}
	${QtMOCSourceFiles}
	${QtCompiledResources})
target_link_libraries(Polygamma ${PYTHON_LIBRARIES})
target_link_libraries(Polygamma ${Boost_LIBRARIES})
target_link_libraries(Polygamma ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Polygamma Qt5::Widgets Qt5::Core)
target_link_libraries(Polygamma avcodec avformat avutil swscale swresample)
target_link_libraries(Polygamma SDL2)
target_compile_features(Polygamma PRIVATE ${StdFeatures})



TARGET = Polygamma

LIBS += -lavcodec -lavformat -lavutil -lswscale \
	-lboost_thread 
	
QT += core gui
QT += widgets

CONFIG += c++11
CONFIG += console
# Use Q_SIGNALS: instead of signals:
#     Q_SIGNAL() instead of signal()
#     Q_SLOTS: instead of slots:
#     Q_SLOT() instead of slot()
#     Function pointers are preferred than macros
CONFIG += no_keywords
# Release build with $ make release
# Debug build with $ make debug
CONFIG += debug_and_release

TEMPLATE = app

unix:!macx {
}
macx {
}
win32 {
}

CONFIG(debug, debug | release) {
	# Debug mode
	DESTDIR = ./debug
} else {
	# Release mode
	DESTDIR = ./release
}

# Hidden folders to store moc/obj/rcc/ui files
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR     = $${DESTDIR}/.moc
RCC_DIR     = $${DESTDIR}/.rcc
UI_DIR      = $${DESTDIR}/.ui

# Auto-generated. Do not edit. All changes will be undone
HEADERS += \
	src/core/Kernel.hpp\
	src/core/polygamma.hpp\
	src/core/Configuration.hpp\
	src/core/Command.hpp\
	src/io/av.hpp\
	src/math/Audio.hpp\
	src/math/Interval.hpp\
	src/math/fourier.hpp\
	src/ui/DialogPreferences.hpp\
	src/ui/ui.hpp\
	src/ui/MainWindow.hpp\
	src/ui/Terminal.hpp\
	src/ui/util/SyntaxHighlighterPython.hpp\
	src/ui/util/ColourButton.hpp\
	src/ui/editors/Editor.hpp\
	src/ui/editors/EditorSimple.hpp\
	src/ui/graphics/Viewport2.hpp\
	src/ui/graphics/Waveform.hpp\
	src/ui/graphics/TickedAxis.hpp\
	src/ui/graphics/Spectrogram.hpp\
	src/ui/panels/PanelBase.hpp
SOURCES += \
	src/main.cpp\
	src/core/Kernel.cpp\
	src/core/Configuration.cpp\
	src/io/av.cpp\
	src/math/Audio.cpp\
	src/math/fourier.cpp\
	src/ui/MainWindow.cpp\
	src/ui/ui.cpp\
	src/ui/Terminal.cpp\
	src/ui/DialogPreferences.cpp\
	src/ui/util/SyntaxHighlighterPython.cpp\
	src/ui/util/ColourButton.cpp\
	src/ui/editors/EditorSimple.cpp\
	src/ui/editors/Editor.cpp\
	src/ui/graphics/TickedAxis.cpp\
	src/ui/graphics/Spectrogram.cpp\
	src/ui/graphics/Viewport2.cpp\
	src/ui/graphics/Waveform.cpp\
	src/ui/panels/PanelBase.cpp
# Auto-generated end

RESOURCES += resources/Polygamma.qrc

# No forms. All UIs are constructed by code.
FORMS +=

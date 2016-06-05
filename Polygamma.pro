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
	src/core/polygamma.hpp\
	src/core/configuration.hpp\
	src/io/av.hpp\
	src/math/interval.hpp\
	src/math/audio.hpp\
	src/math/fourier.hpp\
	src/ui/ui.hpp\
	src/ui/mainwindow.hpp\
	src/ui/editors/editorsimple.hpp\
	src/ui/editors/editor.hpp\
	src/ui/graphics/viewport2.hpp\
	src/ui/graphics/waveform.hpp\
	src/ui/graphics/tickedaxis.hpp\
	src/ui/graphics/spectrogram.hpp\
	src/ui/panels/panelbase.hpp\
	src/ui/panels/panelterminal.hpp
SOURCES += \
	src/main.cpp\
	src/io/av.cpp\
	src/math/fourier.cpp\
	src/math/audio.cpp\
	src/ui/mainwindow.cpp\
	src/ui/ui.cpp\
	src/ui/editors/editorsimple.cpp\
	src/ui/editors/editor.cpp\
	src/ui/graphics/viewport2.cpp\
	src/ui/graphics/spectrogram.cpp\
	src/ui/graphics/waveform.cpp\
	src/ui/graphics/tickedaxis.cpp\
	src/ui/panels/panelbase.cpp\
	src/ui/panels/panelterminal.cpp
# Auto-generated end

# No forms. All UIs are constructed by code.
FORMS +=

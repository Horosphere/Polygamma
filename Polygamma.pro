QT += core gui

TARGET = Polygamma
CONFIG += c++11
CONFIG += console
CONFIG += no_keywords
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
LIBS += -lavcodec -lavformat -lavutil -lswscale

unix:!macx {
# GCC
QMAKE_CXXFLAGS = -Wignored-qualifiers
}
macx {

}
win32 {

}

SOURCES += main.cpp \
    ui/mainwindow.cpp \
    ui/panels/panelbase.cpp \
    io/av.cpp \
    math/audio.cpp \
    ui/graphics/tickedaxis.cpp \
    ui/graphics/waveform.cpp \
    ui/graphics/viewport2.cpp \
    ui/editors/editor.cpp \
    ui/editors/editorsimple.cpp

HEADERS += \
    ui/mainwindow.hpp \
    ui/panels/panelbase.hpp \
    core/polygamma.hpp \
    math/interval.hpp \
    io/av.hpp \
    math/audio.hpp \
    ui/graphics/tickedaxis.hpp \
    ui/graphics/viewport2.hpp \
    ui/graphics/waveform.hpp \
    core/configuration.hpp \
    ui/editors/editor.hpp \
    ui/editors/editorsimple.hpp

# No forms. All UIs are constructed by code.
FORMS +=

include(../src/libtiled/libtiled.pri)

CONFIG += qtestlib
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += . \
    ../src/tiled

macx {
    LIBS += -L$$OUT_PWD/../bin/Tiled.app/Contents/Frameworks
} else {
    LIBS += -L$$OUT_PWD/../lib
}

!win32:!macx {
    QMAKE_RPATHDIR += \$\$ORIGIN/../lib

    # It is not possible to use ORIGIN in QMAKE_RPATHDIR, so a bit manually
    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$$join(QMAKE_RPATHDIR, ":")\'
    QMAKE_RPATHDIR =
}

# Input
SOURCES += test_tmxmapreader.cpp

# Tiled sources to include in the test binary
SOURCES += ../src/tiled/tilesetmanager.cpp \
    ../src/tiled/compression.cpp \
    ../src/tiled/tmxmapreader.cpp
HEADERS += ../src/tiled/tilesetmanager.h

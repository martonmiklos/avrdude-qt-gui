#-------------------------------------------------
#
# Project created by QtCreator 2009-10-12T12:12:02
#
#-------------------------------------------------

TARGET = avrprog
TEMPLATE = app
QT += xml
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

SOURCES += main.cpp\
    partdeschandler.cpp \
    settings.cpp \
    avrprogrammer.cpp \
    avrpart.cpp \
    mainwindow.cpp \
 #   fusemodel.cpp \
 #   fusedelegate.cpp \
    bitfieldmodel.cpp \
    bitfielddelegate.cpp

HEADERS  += \
    partdeschandler.h \
    settings.h \
    avrprogrammer.h \
    avrpart.h \
    mainwindow.h \
#    dudepartnos.h \
    hexlineedit.h \
#    fusemodel.h \
#    fusedelegate.h \
    bitfieldmodel.h \
    bitfielddelegate.h

FORMS    += \
    mainwindow.ui

RESOURCES += \
    icons/icons.qrc

OTHER_FILES += \
    TODO.txt

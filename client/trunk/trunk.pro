######################################################################
# Automatically generated by qmake (2.01a) czw. maj 19 23:10:13 2011
######################################################################

QT += network opengl
TEMPLATE = app
TARGET = tyranny-client 
DEPENDPATH += . src src/ui
INCLUDEPATH += . src

MOC_DIR = moc
UI_DIR = src/ui

# Input
HEADERS += src/mainwindow.h \
    src/prefdialog.h \
    src/iohandler.h \
    src/authdialog.h \
    src/netmanager.h \
    src/packet.h \
    src/protspec.h \
    src/statsdialog.h \
    src/profiledialog.h \
    src/settingsdialog.h \
    src/userlistdialog.h \
    src/rulesdialog.h \
    src/roomdata.h \
    src/gamewindow.h \
    src/boardview.h \
    src/gameprotocol.h \
    src/tokenchooser.h \
    src/turndialog.h
FORMS += src/ui/mainwindow.ui \
    src/ui/prefdialog.ui \
    src/ui/authdialog.ui \
    src/ui/statsdialog.ui \
    src/ui/profiledialog.ui \
    src/ui/settingsdialog.ui \
    src/ui/userlistdialog.ui \
    src/ui/rulesdialog.ui \
    src/ui/tokenchooser.ui \
    src/ui/turndialog.ui \
    src/ui/gamewindow.ui
SOURCES += src/client.cpp src/mainwindow.cpp \
    src/prefdialog.cpp \
    src/iohandler.cpp \
    src/authdialog.cpp \
    src/netmanager.cpp \
    src/packet.cpp \
    src/statsdialog.cpp \
    src/profiledialog.cpp \
    src/settingsdialog.cpp \
    src/userlistdialog.cpp \
    src/rulesdialog.cpp \
    src/gamewindow.cpp \
    src/boardview.cpp \
    src/gameprotocol.cpp \
    src/tokenchooser.cpp \
    src/turndialog.cpp

RESOURCES += \
    resources.qrc

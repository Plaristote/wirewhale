# Add more folders to ship with the application, here
folder_01.source = qml/Wirewhale
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

QT += qml quick

LIBS = /usr/lib/libtins.dylib

CONFIG += c++11

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    qpackettable.cpp \
    qpacket.cpp \
    networkinterfacelist.cpp \
    qpacketlistener.cpp \
    qsnifferthread.cpp

# Installation path
# target.path =

OTHER_FILES += \
    qml/Wirewhale/main.qml \
    qml/Wirewhale/PacketTable.qml \
    qml/Wirewhale/Toolbar.qml

HEADERS += \
    qpackettable.h \
    qpacket.h \
    networkinterfacelist.h \
    qpacketlistener.h \
    qsnifferthread.h

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

CONFIG += c++11

include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    qpackettable.cpp \
    qpacket.cpp \
    networkinterfacelist.cpp \
    qpacketlistener.cpp \
    qsnifferthread.cpp \
    filterprofile.cpp \
    wirewhale.cpp \
    qabstractpacketsniffer.cpp

linux:SOURCES += platforms/linux/qpacketsniffer.cpp \
                 platforms/linux/qpacketsniffer-interface.cpp \
                 platforms/linux/qpacketsniffer-poll.cpp
macx:SOURCES  += platforms/osx/qpacketsniffer.cpp \
                 platforms/osx/qpacketsniffer-poll.cpp
unix:SOURCES  += platforms/unix/qpacketsniffer-packet.cpp \
                 platforms/unix/qunixpacketsniffer.cpp
win32:SOURCES += platforms/windows/qpacketsniffer.cpp

linux:HEADERS += platforms/linux/qpacketsniffer.h
macx:HEADERS  += platforms/osx/qpacketsniffer.h
unix:HEADERS  += platforms/unix/qunixpacketsniffer.h
win32:HEADERS += platforms/windows/qpacketsniffer.h

# Installation path
# target.path =

OTHER_FILES += \
    qml/Wirewhale/main.qml \
    qml/Wirewhale/PacketTable.qml \
    qml/Wirewhale/Toolbar.qml \
    qml/Wirewhale/LabelledComboBox.qml

HEADERS += \
    qpackettable.h \
    qpacket.h \
    networkinterfacelist.h \
    qpacketlistener.h \
    qsnifferthread.h \
    filterprofile.h \
    wirewhale.h \
    qabstractpacketsniffer.h \
    qpacketsniffer.h \
    endianness.h

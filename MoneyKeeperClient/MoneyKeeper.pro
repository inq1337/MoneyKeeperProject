QT       += core gui
QT       += core gui charts
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addcostswindow.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    addcostswindow.h \
    mainwindow.h

FORMS += \
    addcostswindow.ui \
    mainwindow.ui

RESOURCES += \
    resources.qrc

RC_ICONS = icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

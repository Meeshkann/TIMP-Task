QT += core widgets network

CONFIG += c++11
INCLUDEPATH += \
    $$PWD/logindialog_folder \
    $$PWD/mainwindow_folder \
    $$PWD/regdialog_folder

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    logindialog_folder/logindialog.cpp \
    main.cpp \
    mainwindow_folder/mainwindow.cpp \
    regdialog_folder/regdialog.cpp

HEADERS += \
    logindialog_folder/logindialog.h \
    mainwindow_folder/mainwindow.h \
    regdialog_folder/regdialog.h

FORMS += \
    logindialog_folder/logindialog.ui \
    mainwindow_folder/mainwindow.ui \
    regdialog_folder/regdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Images.qrc

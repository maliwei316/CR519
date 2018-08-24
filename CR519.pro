QT += quick
QT += serialbus
QT += sql
QT += network
CONFIG += c++11
QT += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    db_handler.cpp \
    dbh_controller.cpp \
    main.cpp \
    mbc_controller.cpp \
    qmllanguage.cpp \
    worker_modbus.cpp \
    tcp_comm.cpp \
    mainwindow.cpp \
    clstooling.cpp \
    clsbarcode.cpp


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    en_US.qm \
    zh_CN.qm \
    en_US.ts \
    zh_CN.ts \
    img/breakpoint.png \
    img/green-16x16.png \
    img/green.png \
    img/greenStone.png \
    img/ledoff.png \
    img/ledon.png \
    img/menuitem-indicator-hovered.png \
    img/playnormal.png \
    img/record.png \
    img/red.png \
    img/roundbutton-background-checked.png \
    img/roundbutton-background-pressed.png \
    img/run_small.png \
    img/screencap-switch-off.png \
    img/screencap-switch-on.png \
    img/square-green.png \
    img/square-red.png \
    img/square-yellow.png \
    img/statusindicator-active.png \
    img/statusindicator-green.png \
    img/statusindicator-inactive.png \
    img/switch-off.png \
    img/switch-on.png \
    img/togglebutton-checked.png \
    img/togglebutton-unchecked.png \
    img/wb-review-tip.png \
    img/wheel.png \
    img/yellow.png \
    img/yellowStone.png

    Indicator2.qml

HEADERS += \
    bitsoperation.h \
    db_handler.h \
    dbh_controller.h \
    mbc_controller.h \
    myevent.h \
    qmllanguage.h \
    worker_modbus.h \
    tcp_comm.h \
    mainwindow.h \
    clstooling.h \
    clsbarcode.h


FORMS += \
    mainwindow.ui

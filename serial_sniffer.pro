QT       += core gui widgets network serialport

TARGET = qt_serial_sniffer
TEMPLATE = app
CONFIG += c++17

HEADERS  +=   src/mainwindow.h \

SOURCES +=    src/main.cpp \
              src/mainwindow.cpp \

FORMS    +=   src/mainwindow.ui \

RC_ICONS = src/hash.ico

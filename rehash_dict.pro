TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    rehash_dict.c \
    murmurhash.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    rehash_dict.h \
    murmurhash.h


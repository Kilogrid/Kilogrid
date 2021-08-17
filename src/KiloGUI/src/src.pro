QT += widgets core

TEMPLATE = app
TARGET = kilogui

RESOURCES += kilogui.qrc
RC_FILE = kilogui.rc
mac:ICON = images/kilogui.icns

DEFINES += QT_COMPILATION

HEADERS += kilowindow.h \
    calibrate.h \
    ftdiconn.h \
    intelhex.h \
    platform.h \
    configurationparser.h \
    configurationitem.h \
    trackingparser.h \
    mylineedit.h \
    kilogui_info.h \
    program_store.h \
    vusbconn.h \
    serialconn.h

SOURCES += kilowindow.cpp \
    calibrate.cpp \
    ftdiconn.cpp \
    intelhex.cpp \
    kilogui.cpp \
    configurationparser.cpp \
    configurationitem.cpp \
    trackingparser.cpp \
    mylineedit.cpp \
    kilogui_info.cpp \
    program_store.cpp \
    serialconn.cpp \
    vusbconn.cpp

DISTFILES += \
    kilogui.desktop

unix {
  #VARIABLES
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  BINDIR = $$PREFIX/bin
  DATADIR =$$PREFIX/share

  INCLUDEPATH += /usr/local/include \
                 ../../communication \
                 ../../communication/kilogui_dispatcher \
                 ../../communication/dispatcher_module

  LIBS += -L/usr/local/lib -lusb -lftdi

  DEFINES += UNIX

  DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

  CONFIG += c++11
  
  #MAKE INSTALL

  INSTALLS += target desktop icon48 icon64 icon128

  target.path =$$BINDIR

  desktop.path = $$DATADIR/applications
  desktop.files += $${TARGET}.desktop

  icon48.path = $$DATADIR/icons/hicolor/48x48/apps
  icon48.files += images/48x48/kilogui.png

  icon64.path = $$DATADIR/icons/hicolor/64x64/apps
  icon64.files += images/64x64/kilogui.png

  icon128.path = $$DATADIR/icons/hicolor/128x128/apps
  icon128.files += images/128x128/kilogui.png
}

macx {
    INCLUDEPATH += /usr/local/include \
                   ../../communication \
                   ../../communication/kilogui-dispatcher \
                   ../../communication/dispatcher-module \
                   /usr/local/include/libftdi1 \
                   /usr/local/CrossPack-AVR/include

    LIBS -= -lftdi
    LIBS += -L/usr/local/lib -lftdi1 \
            -L/usr/local/CrossPack-AVR/lib -lusb \
            -L/System/Library/Frameworks/ImageIO.framework/Resources -lJPEG\
            -framework CoreFoundation -framework IOKit

    DEFINES += UNIX

    CONFIG += c++11
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS   += -stdlib=libc++
}

win32 {
    INCLUDEPATH +=  C:\libftdi1-1.1_devkit_mingw32\include\libftdi1 \
                    #C:\libusb-1.0.22\include\libusb-1.0 \
                    C:\libusb-win32-bin-1.2.6.0\include \
                    ../../communication \
                    ../../communication/kilogui_dispatcher \
                    ../../communication/dispatcher_module

    LIBS += C:\libftdi1-1.1_devkit_mingw32\lib\libconfuse.a \
            C:\libftdi1-1.1_devkit_mingw32\lib\libftdi1.dll.a \
            #C:\libusb-1.0.22\MinGW32\dll\libusb-1.0.dll.a \
            C:\libusb-win32-bin-1.2.6.0\lib\gcc\libusb.a

    DEFINES += WINDOWS
}

#win32:LIBS += -L$$PWD/C:/libusb_v1.2.6.0/libusb/lib/gcc/

#INCLUDEPATH += $$PWD/C:/libusb_v1.2.6.0/libusb/include
#DEPENDPATH += $$PWD/C:/libusb_v1.2.6.0/libusb/include

#win32:LIBS += -L$$PWD/C:/libftdi1-1.1_devkit_mingw32/lib/libftdi1.a

#INCLUDEPATH += $$PWD/C:/libftdi1-1.1_devkit_mingw32/include/libftdi1
#DEPENDPATH += $$PWD/C:/libftdi1-1.1_devkit_mingw32/include/libftdi1

#win32:LIBS += -L$$PWD/C:/libftdi1-1.1_devkit_mingw32/lib/

#INCLUDEPATH += $$PWD/C:/libftdi1-1.1_devkit_mingw32/include/libusb-1.0
#DEPENDPATH += $$PWD/C:/libftdi1-1.1_devkit_mingw32/include/libusb-1.0

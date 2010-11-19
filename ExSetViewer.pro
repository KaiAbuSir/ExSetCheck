#-------------------------------------------------
#
# Project created by QtCreator 2009-12-10T18:08:35
#
#-------------------------------------------------

TARGET = ExSetViewer
TEMPLATE = app

INCLUDEPATH += incl
INCLUDEPATH += ../EncLib/incl

win32 {
DEFINES += _CRT_SECURE_NO_WARNINGS
}

#rem: EncLib is build with different compilers!
win32 {
CONFIG(release, debug|release){
#  LIBS += -L../EncLib-build/$${QMAKE_CXX}/release   #kai: rel.Path: EncLib cannot be loaded!
  LIBS += -LD:/Projekte/EncLib-build/$${QMAKE_CXX}/release
  message("Building release ")
}
CONFIG(debug, debug|release){
#  LIBS += -L../EncLib-build/$${QMAKE_CXX}/debug #kai: rel.Path: EncLib cannot be loaded!
  LIBS += -LD:/Projekte/EncLib-build/$${QMAKE_CXX}/debug
  message("Building debug")
}
}
unix {
    CONFIG(release, debug|release){
      LIBS += -L../EncLib-build/$${QMAKE_CXX}/release
    }
    CONFIG(debug, debug|release){
      LIBS += -L../EncLib-build/$${QMAKE_CXX}/debug
    }
}
message(" Project Path: " $$PWD )
message(qmake version: " $$QMAKE_QMAKE " - Compiler: "  $$QMAKE_CXX " - Spec: " $${QMAKESPEC} )

LIBS += -lEncLib

SOURCES += src/main.cpp\
        src/mainwindow.cpp

HEADERS  += incl/mytexts.h \
    incl/mainwindow.h \
    incl/globals.h


TARGET=qrcoder
FORMS = QRCoder.ui
HEADERS = QRCoder.h
SOURCES += main.cpp QRCoder.cpp
QMAKE_CXXFLAGS_WARN_OFF= -Wunused-parameter
QT += uitools widgets printsupport
LIBS += -lqrencode

  
  

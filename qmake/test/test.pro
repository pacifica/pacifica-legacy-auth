QT = 
TARGET = testpacificaauth
SOURCES =  test.c

macx: {
	INCLUDEPATH += ../lib
	LIBS += -L../lib/build/Debug
}

unix:!macx {
	INCLUDEPATH += ../lib
	LIBS += -L../lib
}
win32 {
	CONFIG += release
	INCLUDEPATH += ..\\lib
	LIBS += -L..\\lib\\release -L../lib/release
	target.path = "..\\..\\build"
}

CONFIG += console
LIBS += -lpacificaauth
INSTALLS += target

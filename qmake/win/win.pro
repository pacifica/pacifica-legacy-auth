QMAKE_CFLAGS_RELEASE = -static
QMAKE_LFLAGS_RELEASE = -static

QT =
SOURCES =  main.c
TARGET = pathhelper

win32 {
	CONFIG      += release
	target.path = "..\\..\\build\\release"
}

INSTALLS += target

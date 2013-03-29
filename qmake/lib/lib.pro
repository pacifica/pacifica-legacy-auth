QT =
TARGET = pacificaauth
TEMPLATE = lib
CONFIG  += dll
DEFINES =

HEADERS = pacificaauth.h
header_files.files = $$HEADERS
INSTALLS += header_files

unix {
	SOURCES = unix.c
	header_files.path = /usr/include
}
unix:contains(QMAKE_HOST.arch, x86_64): {
	target.path = /usr/lib64
}
unix:!contains(QMAKE_HOST.arch, x86_64): {
	target.path = /usr/lib
}
win32 {
	CONFIG += release
	SOURCES = win.c
	target.path = "..\\..\\build\\bin"
	header_files.path = ..\\..\\build\\include
}

INSTALLS += target

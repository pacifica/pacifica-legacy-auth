QT     += webkit network xml

HEADERS = mainwindow.h services.h kerberos.h gui.h
SOURCES =  main.cpp mainwindow.cpp services.cpp kerberos.cpp gui.cpp
FORMS   = authenticationdialog.ui pacificaauth.ui

unix {
	LIBS        += -lcurl
	target.path = /usr/bin
}
win32 {
	CONFIG      += release link_pkgconfig
	PKGCONFIG   = libcurl
	target.path = "..\\..\\build\\release"
}

INSTALLS += target

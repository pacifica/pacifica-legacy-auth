TEMPLATE = subdirs
SUBDIRS = lib pacificaauth test
win32 {
	SUBDIRS += win
}

VERSION=0.6.0

UNAME=$(shell uname)

all: build-all

include Makefile.${UNAME}

pacificaauth.spec: pacificaauth.spec.in
	sed "s/@VERSION@/$(VERSION)/g" < pacificaauth.spec.in > pacificaauth.spec

pacificaauth.pc: pacificaauth.pc.in
	sed "s/@VERSION@/$(VERSION)/g" < pacificaauth.pc.in > pacificaauth.pc

dist: clean pacificaauth.spec
	rm -f pacificaauth-$(VERSION)
	ln -s . pacificaauth-$(VERSION)
	tar --exclude '.svn' --exclude '*.tar.*' --exclude 'pacificaauth-*/pacificaauth-*' -zcvf pacificaauth-$(VERSION).tar.gz pacificaauth-$(VERSION)/*

RPMOPTIONS=

rpm: dist
	mkdir -p packages/bin packages/src
	rpmbuild --define '_rpmdir '`pwd`'/packages/bin' --define '_srcrpmdir '`pwd`'/packages/src' $(RPMOPTIONS) -ta pacificaauth-$(VERSION).tar.gz

rpms: rpm

clean:
	cd qmake && make distclean || true
	rm -rf build
	rm -f pacificaauth*.wixobj
	rm -f pacificaauth*.wixpdb
	rm -f pacificaauth*.msi
	rm -f pacificaauth*.exe
	rm -f pacificaauth.spec
	rm -f pacificaauth-$(VERSION)
	rm -f pacificaauth-$(VERSION).tar.gz

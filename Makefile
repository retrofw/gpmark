# Project: GPmarkGP2X
# Makefile created by Dev-C++ 4.9.9.2; RetroGame by pingflood

CHAINPREFIX := /opt/mipsel-linux-uclibc
CROSS_COMPILE := $(CHAINPREFIX)/usr/bin/mipsel-linux-

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip

SYSROOT     := $(shell $(CC) --print-sysroot)
SDL_CONFIG = $(SYSROOT)/usr/bin/sdl-config

OBJ  = src/objs/bitfonts.o src/objs/blitting.o src/objs/bunny3d.o src/objs/engine3d.o src/objs/env1.o src/objs/generate3d.o src/objs/main.o src/objs/plasma.o src/objs/radialblur.o src/objs/render3d.o src/objs/rotozoomer.o src/objs/sky1.o
LINKOBJ  = src/objs/bitfonts.o src/objs/blitting.o src/objs/bunny3d.o src/objs/engine3d.o src/objs/env1.o src/objs/generate3d.o src/objs/main.o src/objs/plasma.o src/objs/radialblur.o src/objs/render3d.o src/objs/rotozoomer.o src/objs/sky1.o
LIBS =  `$(SDL_CONFIG) --libs` -s
INCS =  -I$(SYSROOT)/usr/include  -I$(SYSROOT)/usr/lib  -I$(SYSROOT)/lib -I./src
CXXINCS =  $(INCS)
BIN  = gpmark/gpmark.dge
CXXFLAGS = $(CXXINCS)   -DGP2X -fexpensive-optimizations -O3
CFLAGS = $(INCS)   -DGP2X -fexpensive-optimizations -O3
RM = rm -f
MKDIR = mkdir -p

.PHONY: all all-before all-after clean clean-custom

all: all-before gpmark/gpmark.dge all-after

all-before:
	@$(MKDIR) "src/objs" "gpmark"

ipk: all
	@rm -rf /tmp/.gpmark-ipk/ && mkdir -p /tmp/.gpmark-ipk/root/home/retrofw/apps/gpmark /tmp/.gpmark-ipk/root/home/retrofw/apps/gmenu2x/sections/applications
	@cp -r gpmark/gpmark.dge gpmark/gpmark.png gpmark/bunnybig.3do gpmark/draculf.bin /tmp/.gpmark-ipk/root/home/retrofw/apps/gpmark
	@cp gpmark/gpmark.lnk /tmp/.gpmark-ipk/root/home/retrofw/apps/gmenu2x/sections/applications
	@sed "s/^Version:.*/Version: $$(date +%Y%m%d)/" gpmark/control > /tmp/.gpmark-ipk/control
	@tar --owner=0 --group=0 -czvf /tmp/.gpmark-ipk/control.tar.gz -C /tmp/.gpmark-ipk/ control
	@tar --owner=0 --group=0 -czvf /tmp/.gpmark-ipk/data.tar.gz -C /tmp/.gpmark-ipk/root/ .
	@echo 2.0 > /tmp/.gpmark-ipk/debian-binary
	@ar r gpmark/gpmark.ipk /tmp/.gpmark-ipk/control.tar.gz /tmp/.gpmark-ipk/data.tar.gz /tmp/.gpmark-ipk/debian-binary

clean: clean-custom
	${RM} $(OBJ) $(BIN) gpmark/gpmark.ipk

$(BIN): $(OBJ)
	$(CXX) $(LINKOBJ) -o $@ $(LIBS)

src/objs/bitfonts.o: src/bitfonts.cpp
	$(CXX) -c src/bitfonts.cpp -o src/objs/bitfonts.o $(CXXFLAGS)

src/objs/blitting.o: src/blitting.cpp
	$(CXX) -c src/blitting.cpp -o src/objs/blitting.o $(CXXFLAGS)

src/objs/bunny3d.o: src/bunny3d.cpp
	$(CXX) -c src/bunny3d.cpp -o src/objs/bunny3d.o $(CXXFLAGS)

src/objs/engine3d.o: src/engine3d.cpp
	$(CXX) -c src/engine3d.cpp -o src/objs/engine3d.o $(CXXFLAGS)

src/objs/env1.o: src/env1.cpp
	$(CXX) -c src/env1.cpp -o src/objs/env1.o $(CXXFLAGS)

src/objs/generate3d.o: src/generate3d.cpp
	$(CXX) -c src/generate3d.cpp -o src/objs/generate3d.o $(CXXFLAGS)

src/objs/main.o: src/main.cpp
	$(CXX) -c src/main.cpp -o src/objs/main.o $(CXXFLAGS)

src/objs/plasma.o: src/plasma.cpp
	$(CXX) -c src/plasma.cpp -o src/objs/plasma.o $(CXXFLAGS)

src/objs/radialblur.o: src/radialblur.cpp
	$(CXX) -c src/radialblur.cpp -o src/objs/radialblur.o $(CXXFLAGS)

src/objs/render3d.o: src/render3d.cpp
	$(CXX) -c src/render3d.cpp -o src/objs/render3d.o $(CXXFLAGS)

src/objs/rotozoomer.o: src/rotozoomer.cpp
	$(CXX) -c src/rotozoomer.cpp -o src/objs/rotozoomer.o $(CXXFLAGS)

src/objs/sky1.o: src/sky1.cpp
	$(CXX) -c src/sky1.cpp -o src/objs/sky1.o $(CXXFLAGS)

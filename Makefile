# Project: GPmarkGP2X
# Makefile created by Dev-C++ 4.9.9.2; RetroGame by pingflood

CHAINPREFIX := /opt/mipsel-linux-uclibc
CROSS_COMPILE := $(CHAINPREFIX)/usr/bin/mipsel-linux-

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip

SYSROOT     := $(shell $(CC) --print-sysroot)
SDL_CONFIG = $(SYSROOT)/usr/bin/sdl-config

OBJ  = src/gp2x/bitfonts.o src/gp2x/blitting.o src/gp2x/bunny3d.o src/gp2x/engine3d.o src/gp2x/env1.o src/gp2x/generate3d.o src/gp2x/main.o src/gp2x/plasma.o src/gp2x/radialblur.o src/gp2x/render3d.o src/gp2x/rotozoomer.o src/gp2x/sky1.o
LINKOBJ  = src/gp2x/bitfonts.o src/gp2x/blitting.o src/gp2x/bunny3d.o src/gp2x/engine3d.o src/gp2x/env1.o src/gp2x/generate3d.o src/gp2x/main.o src/gp2x/plasma.o src/gp2x/radialblur.o src/gp2x/render3d.o src/gp2x/rotozoomer.o src/gp2x/sky1.o
LIBS =  `$(SDL_CONFIG) --libs` -s
INCS =  -I$(SYSROOT)/usr/include  -I$(SYSROOT)/usr/lib  -I$(SYSROOT)/lib
CXXINCS =  $(INCS)
BIN  = gpmark/gpmark.dge
CXXFLAGS = $(CXXINCS)   -DGP2X -fexpensive-optimizations -O3
CFLAGS = $(INCS)   -DGP2X -fexpensive-optimizations -O3
RM = rm -f
MKDIR = mkdir -p

.PHONY: all all-before all-after clean clean-custom

all: all-before gpmark/gpmark.dge all-after

all-before:
	$(MKDIR) "src/objs" "gpmark"

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
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LINKOBJ) -o $@ $(LIBS)

src/objs/bitfonts.o: bitfonts.cpp
	$(CXX) -c bitfonts.cpp -o src/objs/bitfonts.o $(CXXFLAGS)

src/objs/blitting.o: blitting.cpp
	$(CXX) -c blitting.cpp -o src/objs/blitting.o $(CXXFLAGS)

src/objs/bunny3d.o: bunny3d.cpp
	$(CXX) -c bunny3d.cpp -o src/objs/bunny3d.o $(CXXFLAGS)

src/objs/engine3d.o: engine3d.cpp
	$(CXX) -c engine3d.cpp -o src/objs/engine3d.o $(CXXFLAGS)

src/objs/env1.o: env1.cpp
	$(CXX) -c env1.cpp -o src/objs/env1.o $(CXXFLAGS)

src/objs/generate3d.o: generate3d.cpp
	$(CXX) -c generate3d.cpp -o src/objs/generate3d.o $(CXXFLAGS)

src/objs/main.o: main.cpp
	$(CXX) -c main.cpp -o src/objs/main.o $(CXXFLAGS)

src/objs/plasma.o: plasma.cpp
	$(CXX) -c plasma.cpp -o src/objs/plasma.o $(CXXFLAGS)

src/objs/radialblur.o: radialblur.cpp
	$(CXX) -c radialblur.cpp -o src/objs/radialblur.o $(CXXFLAGS)

src/objs/render3d.o: render3d.cpp
	$(CXX) -c render3d.cpp -o src/objs/render3d.o $(CXXFLAGS)

src/objs/rotozoomer.o: rotozoomer.cpp
	$(CXX) -c rotozoomer.cpp -o src/objs/rotozoomer.o $(CXXFLAGS)

src/objs/sky1.o: sky1.cpp
	$(CXX) -c sky1.cpp -o src/objs/sky1.o $(CXXFLAGS)

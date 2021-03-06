HOST:=i386-linux
TARGET:=arm-linux
CROSS-COMPIL=$(TARGET)-

ROOT_DIR:=$(shell pwd)

CONFIG-OPTS:=--build=$(HOST) --host=$(TARGET)

INSTALL_DIR:=$(ROOT_DIR)/../build/

PREFIX=/usr/local

LIBDIR=$(INSTALL_DIR)/$(PREFIX)/lib
INCDIR=$(INSTALL_DIR)/$(PREFIX)/include

LDFLAGS=-L$(LIBDIR)
CFLAGS=-I$(INCDIR)

CONFIGURE=LDFLAGS=$(LDFLAGS) CFLAGS=$(CFLAGS)  ./configure
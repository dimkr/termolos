CC ?= cc
CFLAGS ?= -O2 -pipe
LDFLAGS ?= -Wl,-s
DESTDIR ?=
BIN_DIR ?= /bin
MAN_DIR ?= /usr/share/man
DOC_DIR ?= /usr/share/doc

CFLAGS += -Wall -pedantic

INSTALL = install -v

SRCS = $(wildcard *.c)
OBJECTS = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h)

all: termolos

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

termolos: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

install: all
	$(INSTALL) -D -m 755 termolos $(DESTDIR)/$(BIN_DIR)/termolos
	$(INSTALL) -D -m 644 termolos.1 $(DESTDIR)/$(MAN_DIR)/man1/termolos.1
	$(INSTALL) -D -m 644 README $(DESTDIR)/$(DOC_DIR)/shus/README
	$(INSTALL) -m 644 AUTHORS $(DESTDIR)/$(DOC_DIR)/shus/AUTHORS
	$(INSTALL) -m 644 COPYING $(DESTDIR)/$(DOC_DIR)/shus/COPYING

clean:
	rm -f termolos $(OBJECTS)

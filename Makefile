CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter -g

CC_SOURCES = \
src/main.c \
src/menu.c \
src/templates.c \
src/database.c \
src/style.c

HEADERS = \
include/main.h \
include/menu.h \
include/templates.h \
include/database.h \
include/style.h

EXECUTABLE = bencode.bin

OBJECTS = $(CC_SOURCES:.c=.o)

all: $(EXECUTABLE)
	rm -f $(OBJECTS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -lcurl -l sqlite3

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

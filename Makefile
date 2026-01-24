CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter -g

CC_SOURCES = \
Core/Src/main.c \
Core/Src/api.c \
Core/Src/menu.c \
Core/Src/templates.c \
Core/Src/database.c \
Core/Src/style.c \
Core/Src/cJSON.c

#Core/Inc/api.h

HEADERS = \
Core/Inc/main.h \
Core/Src/menu.h \
Core/Src/templates.h \
Core/Inc/database.h \
Core/Inc/style.h \
Core/Inc/cJSON.h
#Core/Inc/bencode.h \
#Core/Inc/bencode_utils.h \
#Core/Inc/main.h \
#Core/Inc/codes.h

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

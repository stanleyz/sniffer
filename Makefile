CC=gcc
CFLAGS=-g3 -Wno-unused-result

#SOURCES=$(shell find . -type f -iname '*.c' | grep -v 'test')
#S_OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)
#S_SOURCES=sniffer.c packet.c lib/common.c tcp/http.c
S_OBJECTS=sniffer.o packet.o lib/string.o tcp/http.o
S_TARGET=sniffer
S_LIBS=-lpcap

A_OBJECTS=analyzer.o tcp/https.o processer.o lib/string.o lib/network.o lib/mysql.o
A_TARGET=analyzer
A_LIBS=-lssl -lcrypto

MYSQL_LIBS=$(shell mysql_config --cflags --libs_r)

all: $(S_TARGET) $(A_TARGET)

$(S_TARGET): $(S_OBJECTS)
	@echo
	@echo Generating $(S_TARGET) now...
	$(CC) $(CFLAGS) $^ -o $@ $(S_LIBS)

$(A_TARGET): $(A_OBJECTS)
	@echo
	@echo Generating $(A_TARGET) now...
	$(CC) $(CFLAGS) $^ -o $@ $(A_LIBS) $(MYSQL_LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(S_OBJECTS) $(A_OBJECTS) $(S_TARGET) $(A_TARGET)

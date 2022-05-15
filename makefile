CPPFILES=$(shell find . -name '*.cpp')
CFILES=$(shell find . -name '*.c')
ASMFILES=$(shell find . -name '*.s')
OBJS = $(CPPFILES:.cpp=.cpp.o) $(CFILES:.c=.c.o) $(ASMFILES:.s=.s.o)

CPPFLAGS=-std=c++17 -I . -lssl -lcrypto -pthread -ggdb
ASMFLAGS=
CFLAGS=

OUT=../bin/server
GPP=g++-9
GCC=gcc-9
NASM=nasm

$(OUT): $(OBJS)
	$(GPP) $(OBJS) $(CPPFLAGS) $(CXXFLAGS) -o $@

%.cpp.o: %.cpp
	$(GPP) $(CPPFLAGS) $< -o $@ -c

%.c.o: %.c
	$(GCC) $(CFLAGS) $< -o $@ -c

%.s.o: %.s
	$(NASM) $(ASMFLAGS) $< -o $@

.PHONY: clean

clean:
	$(RM) -rf *.o $(shell find . -name '*.o') $(OUT)

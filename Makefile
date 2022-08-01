all: dolorem

CC = gcc
MODE ?= -g -O0
CFLAGS ?=
LINKFLAGS ?=

ifdef TCC
	TCCFLAGS=-ltcc -DTCC
endif

%.o: %.c *.h
	$(CC) -c -Wall -Wextra -fpic $< -o $@ $(MODE) $(CFLAGS) $(TCCFLAGS)

libdolorem.so: list.o cgen.o hashmap.o include.o main.o lower.o os_detection.o
	$(CC) $^ -o $@ -ldl -shared $(LINKFLAGS) $(TCCFLAGS)

dolorem: main.o libdolorem.so
	$(CC) -L. $^ -o $@ -ldl $(MODE) $(LINKFLAGS)

.PHONY: report clean
report:
	@echo Using C compiler: $(CC)
	@echo Using C++/LLVM compiler: $(CXX)
	@echo Debug/Release flags: $(MODE)
	@echo Other flags \(CFLAGS\): $(CFLAGS)
	@echo Other flags \(CXXFLAGS\): $(CXXFLAGS)
	@echo Other flags \(LINKFLAGS\): $(LINKFLAGS)

clean:
	rm *.o || true
	rm test || true
	rm libtest.so || true

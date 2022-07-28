all: dolorem

CC = gcc
MODE ?= -g -O0
CFLAGS ?=
LINKFLAGS ?=

%.o: %.c *.h
	$(CC) -c -Wall -Wextra -fpic -std=c11 $< -o $@ $(MODE) $(CFLAGS)

libdolorem.so: list.o cgen.o hashmap.o include.o main.o lower.o os_detection.o
	$(CC) $^ -o $@ -ldl -shared $(LINKFLAGS)

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

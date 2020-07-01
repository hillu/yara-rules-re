CFLAGS += $(shell pkg-config --cflags yara)
LDFLAGS += $(shell pkg-config --libs yara)

all: yara-disasm

yara-disasm: main.o yrvm.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f *.o yara-disasm

.PHONY: all clean

CC=cc
CFLAG=-O2 -Wall -std=c89
OBJS=bitstream.o huffman.o main.o
BIN=huffman

all: $(BIN)

huffman: $(OBJS)
	@echo "BUILD  $@"
	@$(CC) -o $@ $^

%.o: %.c
	@echo "CC     $<"
	@$(CC) -c $<

.PHONY:
clean:
	@echo "clean  $(BIN) $(OBJS)"
	@rm -f $(BIN) $(OBJS)

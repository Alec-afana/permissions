CC = gcc
CFLAGS = -O2 -pg -Wall -Iinclude
LDFLAGS = 

SRC = src/btree.c src/access_ops.c src/acl_file.c src/acl_manager.c \
      src/acl_check.c src/acl_update.c src/fs_ops.c src/main.c \
      src/scale_tests.c
OBJ = $(SRC:.c=.o)

BIN = myfs

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

test: all
	./tests/run_tests.sh

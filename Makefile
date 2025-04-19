# Makefile

CC      := gcc
CFLAGS  := -O2 -Wall -Iinclude -pg
LDFLAGS := -pg

# Общие исходники
COMMON_SRCS := \
    src/btree.c \
    src/access_ops.c \
    src/acl_manager.c \
    src/acl_file.c \
    src/acl_check.c \
    src/acl_update.c \
    src/fs_ops.c

COMMON_OBJS := $(COMMON_SRCS:.c=.o)

# Основная программа (myfs) — используем main.c + scale_tests.c
MYFS_SRCS := src/main.c src/scale_tests.c
MYFS_OBJS := $(MYFS_SRCS:.c=.o)

# Новый тест только для ACL-файлов
ACL_TEST_SRCS := src/scale_acl_tests.c
ACL_TEST_OBJS := $(ACL_TEST_SRCS:.c=.o)

.PHONY: all clean

all: myfs scale_acl_tests

# Собираем myfs (главное приложение + тест run_scale_tests)
myfs: $(COMMON_OBJS) $(MYFS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Собираем отдельный скоростной тест ACL
scale_acl_tests: $(COMMON_OBJS) $(ACL_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Шаблон для сборки .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o myfs scale_acl_tests gmon.out

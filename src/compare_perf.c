#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <stdlib.h>  // для strtoul, malloc, free
#include <stdint.h>
#include <sys/stat.h>
#include "acl_file.h"  // для read_acl_from_file
#include "acl_manager.h"


static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main(int argc, char **argv) {
    size_t N = 100000;
    if (argc > 1) N = strtoul(argv[1], NULL, 10);

    // 1) Тест stat() для ACL-файлов
    uint64_t t0 = now_ns();
    for (size_t i = 0; i < N; i++) {
        uint64_t idx = 100000000 + i;
        char path[256];
        get_acl_path_murmur(idx, path, sizeof(path));
        // путь относительный, допишем data/
        char full[512];
        snprintf(full, sizeof(full), "data/%s", path);
        struct stat st;
        if (stat(full, &st) < 0) {
            perror("stat acl");
        }
    }
    uint64_t t1 = now_ns();
    printf("stat() на %zu ACL-файлов: %llu мс\n",
           N, (unsigned long long)(t1 - t0) / 1000000ULL);

    // 2) Тест вашего API: чтение ACL
    t0 = now_ns();
    for (size_t i = 0; i < N; i++) {
        uint64_t idx = 100000000 + i;
        my_acl_t *acl = read_acl_from_file(idx);
        // сразу освобождаем
        if (acl) {
            free(acl->entries);
            free(acl);
        }
    }
    t1 = now_ns();
    printf("read_acl_from_file на %zu ACL-файлов: %llu мс\n",
           N, (unsigned long long)(t1 - t0) / 1000000ULL);

    // 3) Тест stat() на userfiles
    t0 = now_ns();
    for (size_t i = 0; i < N; i++) {
        char full[512];
        snprintf(full, sizeof(full), "data/userfiles/userfile_%zu.dat", i);
        struct stat st;
        if (stat(full, &st) < 0) {
            //perror("stat userfile");
        }
    }
    t1 = now_ns();
    printf("stat() на %zu userfiles: %llu мс\n",
           N, (unsigned long long)(t1 - t0) / 1000000ULL);

    return 0;
}

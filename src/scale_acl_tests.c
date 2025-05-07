#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "acl_file.h"
#include "my_acl.h"

static uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main(int argc, char *argv[]) {
    size_t num = 100000;  // по умолчанию 100 000
    if (argc > 1) num = strtoul(argv[1], NULL, 10);
    printf("Scale-ACL-test: создаём %zu ACL-файлов\n", num);

    // Простенький ACL из 2 записей
    my_acl_t acl;
    acl.count = 2;
    acl.entries = malloc(2 * sizeof(acl_entry_t));
    acl.entries[0].tag = ACL_USER_OBJ;
    acl.entries[0].id   = 0;
    acl.entries[0].perms= 7;
    acl.entries[1].tag = ACL_OTHER;
    acl.entries[1].id   = 0;
    acl.entries[1].perms= 5;

    uint64_t t0 = get_time_ns();
    for (size_t i = 0; i < num; i++) {
        uint64_t idx = i; //100000000ULL + i;
	//fprintf(stderr, "запись ACL для idx=%llu\n",
        //            (unsigned long long)idx);
        if (write_acl_to_file(idx, &acl) != 0) {
            fprintf(stderr, "Ошибка записи ACL для idx=%llu\n",
                    (unsigned long long)idx);
        }
    }
    uint64_t t1 = get_time_ns();
    printf("Время создания %zu ACL-файлов: %llu нс\n",
           num, (unsigned long long)(t1 - t0));
    free(acl.entries);
    return 0;
}

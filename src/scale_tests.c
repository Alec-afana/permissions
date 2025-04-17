#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "fs_ops.h"
#include "acl_file.h"
#include "acl_update.h"
#include "btree.h"
#include "my_acl.h"

// Предполагается, что глобальное дерево объявлено в main.c
extern BTree global_tree;

uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

void run_scale_tests(int num_files) {
    printf("\nЗапуск масштабного теста для %d файлов...\n", num_files);

    if (custom_mkdir("userfiles") != 0) {
        printf("Ошибка создания каталога userfiles\n");
        return;
    }

    uint64_t t_start = get_time_ns();
    for (int i = 0; i < num_files; i++) {
        uint64_t index = 0x10000000ULL + i;

        // Создаем метаданные
        file_metadata_t *meta = malloc(sizeof(file_metadata_t));
        if (!meta) continue;
        meta->index = index;
        meta->uid = 1000;
        meta->gid = 1000;
        meta->mode = 0644;
        meta->acl = NULL;
        
        btree_insert(&global_tree, meta);

        // Создаем тестовый ACL (2 записи)
        my_acl_t *acl = malloc(sizeof(my_acl_t));
        if (!acl) { free(meta); continue; }
        acl->count = 2;
        acl->entries = malloc(2 * sizeof(acl_entry_t));
        if (!acl->entries) { free(acl); free(meta); continue; }
        acl->entries[0].tag = ACL_USER_OBJ;
        acl->entries[0].id = 1000;
        acl->entries[0].perms = 7; // rwx
        acl->entries[1].tag = ACL_OTHER;
        acl->entries[1].id = 0;
        acl->entries[1].perms = 5; // r-x

        if (myfs_update_acl(index, 1000, 7) != 0) {
            printf("Ошибка обновления ACL для файла с индексом %lu\n", index);
        }

        free(acl->entries);
        free(acl);

        // Создаем объемный файл
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "userfiles/file_%lu.dat", index);
        if (custom_create_file(file_path) != 0) {
            printf("Ошибка создания файла %s\n", file_path);
        } else {
            size_t size = 1024 * 1024;
            char *buffer = malloc(size);
            if (buffer) {
                memset(buffer, 'A', size);
                if (custom_write_file(file_path, buffer, size) != 0) {
                    printf("Ошибка записи файла %s\n", file_path);
                }
                free(buffer);
            }
        }
    }
    uint64_t t_end = get_time_ns();
    printf("Масштабный тест для %d файлов завершен. Общее время: %lu нс\n", num_files, (unsigned long)(t_end - t_start));
}

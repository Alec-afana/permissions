#include "acl_file.h"
#include "acl_manager.h"
#include "fs_ops.h"
#include "my_acl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

int write_acl_to_file(uint64_t index, my_acl_t *acl) {
    char path[256];
    get_acl_path_murmur(index, path, sizeof(path));

    // Создаем необходимые директории: "data", "data/acl", "data/acl/<XX>", "data/acl/<XX>/<YY>"
    mkdir("data", 0755);
    mkdir("data/acl", 0755);
    
    // Для создания уровней используем тот же алгоритм, что и в get_acl_path_murmur:
    char index_str[32];
    snprintf(index_str, sizeof(index_str), "%llu", (unsigned long long)index);
    uint32_t hash = murmurhash2(index_str, (int)strlen(index_str), 0x9747b28c);
    unsigned int level1 = (hash >> 24) & 0xFF;
    unsigned int level2 = (hash >> 16) & 0xFF;
    char dir1[64], dir2[128];
    snprintf(dir1, sizeof(dir1), "data/acl/%02X", level1);
    snprintf(dir2, sizeof(dir2), "%s/%02X", dir1, level2);
    mkdir(dir1, 0755);
    mkdir(dir2, 0755);

    FILE *file = fopen(path, "wb");
    if (!file) {
        perror("Ошибка открытия файла ACL");
        return -1;
    }
    fwrite(&acl->count, sizeof(int), 1, file);
    fwrite(acl->entries, sizeof(acl_entry_t), acl->count, file);
    fclose(file);
    //printf("ACL успешно записан: %s\n", path);
    return 0;
}

my_acl_t *read_acl_from_file(uint64_t index) {
    char path[256];
    get_acl_path_murmur(index, path, sizeof(path));

    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Ошибка открытия файла ACL");
        return NULL;
    }
    int count;
    if (fread(&count, sizeof(int), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Ошибка чтения размера ACL-файла\n");
        return NULL;
    }
    if (count <= 0) {
        fclose(file);
        fprintf(stderr, "Ошибка: некорректное количество ACL-записей\n");
        return NULL;
    }
    my_acl_t *acl = malloc(sizeof(my_acl_t));
    if (!acl) {
        fclose(file);
        return NULL;
    }
    acl->count = count;
    acl->entries = malloc(count * sizeof(acl_entry_t));
    if (!acl->entries) {
        free(acl);
        fclose(file);
        return NULL;
    }
    if (fread(acl->entries, sizeof(acl_entry_t), count, file) != (size_t)count) {
        free(acl->entries);
        free(acl);
        fclose(file);
        fprintf(stderr, "Ошибка чтения ACL-записей\n");
        return NULL;
    }
    fclose(file);
    //printf("ACL успешно загружен: %s\n", path);
    return acl;
}

int delete_acl_file(uint64_t index) {
    char path[256];
    get_acl_path_murmur(index, path, sizeof(path));
    if (remove(path) == 0) {
        printf("ACL-файл %s успешно удалён.\n", path);
        return 0;
    } else {
        perror("Ошибка удаления ACL-файла");
        return -1;
    }
}

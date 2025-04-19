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

/**
 * Пишет ACL в файл по хешированному пути:
 *   data/acl/XX/YY/<index>.acl
 */
int write_acl_to_file(uint64_t index, my_acl_t *acl) {
    char rel_path[256];
    get_acl_path_murmur(index, rel_path, sizeof(rel_path));
    // rel_path == "acl/XX/YY/<index>.acl"

    // Получаем директорию (всё до последнего '/')
    char dir_path[256];
    strncpy(dir_path, rel_path, sizeof(dir_path));
    char *slash = strrchr(dir_path, '/');
    if (!slash) {
        fprintf(stderr, "Invalid ACL path: %s\n", rel_path);
        return -EINVAL;
    }
    *slash = '\0';  // dir_path == "acl/XX/YY"

    // Создаём recursively: data/acl/.../YY
    if (custom_mkdir(dir_path) < 0) {
        perror("custom_mkdir");
        return -errno;
    }

    // Упаковываем count + entries в один буфер
    size_t hdr_size = sizeof(int);
    size_t entries_size = acl->count * sizeof(acl_entry_t);
    size_t total_size = hdr_size + entries_size;

    char *buf = malloc(total_size);
    if (!buf) {
        fprintf(stderr, "OOM packing ACL\n");
        return -ENOMEM;
    }
    memcpy(buf, &acl->count, hdr_size);
    memcpy(buf + hdr_size, acl->entries, entries_size);

    // Записываем файл в one-shot
    int ret = custom_write_file(rel_path, buf, total_size);
    if (ret < 0) {
        fprintf(stderr, "custom_write_file(%s) failed: %d\n", rel_path, ret);
    }
    free(buf);
    return ret;
}

/**
 * Читает ACL из файла:
 *   data/acl/XX/YY/<index>.acl
 */
my_acl_t *read_acl_from_file(uint64_t index) {
    char rel_path[256];
    get_acl_path_murmur(index, rel_path, sizeof(rel_path));

    size_t file_size;
    char *buf = custom_read_file(rel_path, &file_size);
    if (!buf) {
        fprintf(stderr, "custom_read_file(%s) failed\n", rel_path);
        return NULL;
    }

    // Проверяем, что хотя бы помещается заголовок
    if (file_size < sizeof(int)) {
        fprintf(stderr, "ACL file too small: %s\n", rel_path);
        free(buf);
        return NULL;
    }

    int count;
    memcpy(&count, buf, sizeof(int));
    if (count <= 0) {
        fprintf(stderr, "Invalid ACL count %d in %s\n", count, rel_path);
        free(buf);
        return NULL;
    }

    size_t expected = sizeof(int) + count * sizeof(acl_entry_t);
    if ((size_t)file_size < expected) {
        fprintf(stderr, "ACL file truncated: %s (got %zu, need %zu)\n",
                rel_path, file_size, expected);
        free(buf);
        return NULL;
    }

    my_acl_t *acl = malloc(sizeof(*acl));
    if (!acl) {
        perror("malloc acl");
        free(buf);
        return NULL;
    }
    acl->count = count;
    acl->entries = malloc(count * sizeof(acl_entry_t));
    if (!acl->entries) {
        perror("malloc entries");
        free(acl);
        free(buf);
        return NULL;
    }
    memcpy(acl->entries, buf + sizeof(int), count * sizeof(acl_entry_t));
    free(buf);
    return acl;
}

/**
 * Удаляет ACL-файл по относительному пути, используя custom_remove_file().
 */
int delete_acl_file(uint64_t index) {
    char rel_path[256];
    get_acl_path_murmur(index, rel_path, sizeof(rel_path));
    // rel_path == "acl/XX/YY/<index>.acl"
    int ret = custom_remove_file(rel_path);
    if (ret < 0) {
        fprintf(stderr, "Ошибка удаления ACL-файла %s: %s\n",
                rel_path, strerror(-ret));
    }
    return ret;
}

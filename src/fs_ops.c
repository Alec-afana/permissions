#include "fs_ops.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FS_ROOT "data"//"/mnt/tmpfs_myfs2" //"data"

int custom_mkdir(const char *path) {
    char full_path[512];
    if (strncmp(path, FS_ROOT, strlen(FS_ROOT)) != 0) {
        snprintf(full_path, sizeof(full_path), "%s/%s", FS_ROOT, path);
    } else {
        strncpy(full_path, path, sizeof(full_path)-1);
        full_path[sizeof(full_path)-1] = '\0';
    }
    //printf("custom_mkdir: Creating directory %s\n", full_path);
    // Реализация "mkdir -p": проходим по строке full_path и создаем каждый уровень, когда встречаем '/'
    for (char *p = full_path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';  // временно заменяем '/' на нуль-терминатор
            if (mkdir(full_path, 0755) != 0) {
                if (errno != EEXIST) {
                    perror("custom_mkdir");
                    *p = '/';  // возвращаем символ
                    return -errno;
                }
            }
            *p = '/';  // возвращаем разделитель
        }
    }
    // Создаем последний каталог
    if (mkdir(full_path, 0755) != 0) {
        if (errno != EEXIST) {
            perror("custom_mkdir");
            return -errno;
        }
    }
    return 0;
}

int custom_create_file(const char *path) {
    char full_path[512];
    if (strncmp(path, FS_ROOT, strlen(FS_ROOT)) != 0) {
        snprintf(full_path, sizeof(full_path), "%s/%s", FS_ROOT, path);
    } else {
        strncpy(full_path, path, sizeof(full_path)-1);
        full_path[sizeof(full_path)-1] = '\0';
    }
    //printf("custom_create_file: Creating file %s\n", full_path);
    FILE *fp = fopen(full_path, "w+b");
    if (!fp) {
        perror("custom_create_file fopen");
        return -errno;
    }
    fclose(fp);
    return 0;
}

int custom_write_file(const char *path, const char *data, size_t len) {
    char full_path[512];
    if (strncmp(path, FS_ROOT, strlen(FS_ROOT)) != 0) {
        snprintf(full_path, sizeof(full_path), "%s/%s", FS_ROOT, path);
    } else {
        strncpy(full_path, path, sizeof(full_path)-1);
        full_path[sizeof(full_path)-1] = '\0';
    }
    FILE *fp = fopen(full_path, "w+b");
    if (!fp) {
        perror("custom_write_file fopen");
        return -errno;
    }
    size_t written = fwrite(data, 1, len, fp);
    fclose(fp);
    if (written != len)
        return -EIO;
    return 0;
}

char *custom_read_file(const char *path, size_t *out_size) {
    char full_path[512];
    if (strncmp(path, FS_ROOT, strlen(FS_ROOT)) != 0) {
        snprintf(full_path, sizeof(full_path), "%s/%s", FS_ROOT, path);
    } else {
        strncpy(full_path, path, sizeof(full_path)-1);
        full_path[sizeof(full_path)-1] = '\0';
    }
    FILE *fp = fopen(full_path, "rb");
    if (!fp) {
        perror("custom_read_file fopen");
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    char *buffer = malloc(size);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }
    size_t read_bytes = fread(buffer, 1, size, fp);
    fclose(fp);
    if (read_bytes != size) {
        free(buffer);
        return NULL;
    }
    *out_size = size;
    return buffer;
}


/**
 * Удаляет файл по относительному пути внутри FS_ROOT ("data/").
 * @param path Относительный путь, например "acl/AA/BB/123.acl"
 * @return 0 при успехе, -errno при ошибке
 */
int custom_remove_file(const char *path) {
    char full_path[512];
    // FS_ROOT определён в этом файле как макрос, обычно "data"
    snprintf(full_path, sizeof(full_path), "%s/%s", FS_ROOT, path);
    if (remove(full_path) == 0) {
        return 0;
    } else {
        return -errno;
    }
}

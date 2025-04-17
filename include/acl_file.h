#ifndef ACL_FILE_H
#define ACL_FILE_H

#include <stdint.h>
#include "my_acl.h"

// Генерация пути ACL-файла с использованием MurmurHash2
//void get_acl_path_murmur(uint64_t index, char *out_path, size_t max_len);

// Функция для записи ACL в файл
int write_acl_to_file(uint64_t index, my_acl_t *acl);

// Функция для чтения ACL из файла
my_acl_t *read_acl_from_file(uint64_t index);

// Функция для удаления ACL-файла
int delete_acl_file(uint64_t index);

#endif // ACL_FILE_H

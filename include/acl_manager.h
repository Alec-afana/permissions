#ifndef ACL_MANAGER_H
#define ACL_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include "my_acl.h"

/**
 * Функция get_acl_path_murmur:
 *  Принимает уникальный ключ (например, в виде строки или числа, здесь мы будем использовать индекс, преобразованный в строку)
 *  и возвращает путь для хранения файла ACL.
 *  
 *  Новый путь будет иметь вид:
 *     "data/acl/<XX>/<YY>/<full_index>.acl"
 *  где <XX> и <YY> — первые два байта 32-битного хэша, вычисленного по MurmurHash.
 *
 * @param index: уникальный индекс файла (например, uint64_t, который мы преобразуем в строку)
 * @param out_path: буфер для результата (достаточного размера, например, 256 байт)
 * @param max_len: размер буфера out_path
 */
void get_acl_path_murmur(uint64_t index, char *out_path, size_t max_len);

/**
 * Прототип MurmurHash2 для 32-битного хэша.
 * Реализация находится в acl_manager.c.
 */
uint32_t murmurhash2(const void * key, int len, uint32_t seed);

#endif // ACL_MANAGER_H

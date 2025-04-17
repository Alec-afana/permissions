#include "acl_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Пример реализации MurmurHash2 для 32-битного хэша
// Взято из исходных примеров, с минимальными изменениями.
uint32_t murmurhash2(const void * key, int len, uint32_t seed) {
    // 'm' и 'r' — константы
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t h = seed ^ len;
    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }

    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

// Функция, которая принимает индекс, преобразует его в строку и вычисляет хэш
// Затем использует первые два байта хэша для построения пути
void get_acl_path_murmur(uint64_t index, char *out_path, size_t max_len) {
    // Преобразуем индекс в строку
    char index_str[32];
    snprintf(index_str, sizeof(index_str), "%llu", (unsigned long long)index);
    
    // Вычисляем 32-битный хэш для строки индекса с заданным seed
    uint32_t hash = murmurhash2(index_str, strlen(index_str), 0x9747b28c);
    
    // Извлекаем первые два байта
    unsigned int level1 = (hash >> 24) & 0xFF;
    unsigned int level2 = (hash >> 16) & 0xFF;
    
    // Формируем путь: "data/acl/<XX>/<YY>/<index_str>.acl"
    snprintf(out_path, max_len, "data/acl/%02X/%02X/%s.acl", level1, level2, index_str);
}

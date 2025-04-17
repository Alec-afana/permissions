#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "acl_file.h"
#include "my_acl.h"

/**
 * Функция проверки доступа по ACL.
 * Принимает:
 *   - index: уникальный индекс файла,
 *   - user_str: строка, содержащая числовой идентификатор пользователя.
 * Если найдена запись с тегом ACL_USER_OBJ или ACL_USER, где id совпадает с user_id, возвращает 1 (доступ разрешён),
 * иначе – 0 (доступ запрещён).
 */
int check_acl(uint64_t index, const char *user_str) {
    my_acl_t *acl = read_acl_from_file(index);
    if (!acl) {
        // Если ACL-файл не найден, считаем, что доступа нет
        return 0;
    }

    // Преобразуем строку в целое число
    int user_id = atoi(user_str);

    // Перебираем все записи ACL
    for (int i = 0; i < acl->count; i++) {
        if ((acl->entries[i].tag == ACL_USER_OBJ || acl->entries[i].tag == ACL_USER) &&
            acl->entries[i].id == (uint32_t)user_id) {
            free(acl->entries);
            free(acl);
            return 1; // Доступ разрешён
        }
    }
    free(acl->entries);
    free(acl);
    return 0; // Доступ запрещён
}


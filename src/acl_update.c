#include "acl_update.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "acl_file.h"
#include "my_acl.h"

/**
 * Функция обновления ACL для файла.
 * Принимает:
 *   - index: уникальный индекс файла,
 *   - new_user_id: числовой идентификатор пользователя, для которого обновляем ACL,
 *   - new_perms: новые права доступа.
 * Если ACL для файла существует, добавляет новую запись; если нет – создаёт новый ACL.
 */
int myfs_update_acl(uint64_t index, uint32_t new_user_id, mode_t new_perms) {
    my_acl_t *acl = read_acl_from_file(index);
    if (!acl) {
        // Если ACL не существует, создаем новый ACL
        acl = malloc(sizeof(my_acl_t));
        if (!acl) return -1;
        acl->count = 0;
        acl->entries = NULL;
    }
    
    // Расширяем массив записей, добавляя новую запись
    acl_entry_t *temp = realloc(acl->entries, (acl->count + 1) * sizeof(acl_entry_t));
    if (!temp) {
	free(acl->entries);
        free(acl);
        return -1;
    }
    acl->entries = temp;
    acl->entries[acl->count].tag = ACL_USER;  // или ACL_USER_OBJ, если обновляем владельца
    acl->entries[acl->count].id = new_user_id;
    acl->entries[acl->count].perms = new_perms;
    acl->count++;
    
    int res = write_acl_to_file(index, acl);
    
    free(acl->entries);
    free(acl);
    return res;
}

/**
 * Функция удаления ACL-файла.
 * Просто вызывает delete_acl_file, определенную в acl_file.c.
 */
int remove_acl(uint64_t index) {
    return delete_acl_file(index);
}

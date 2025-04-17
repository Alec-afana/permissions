#ifndef ACL_UPDATE_H
#define ACL_UPDATE_H

#include <stdint.h>
#include "my_acl.h"
//#include "file_metadata.h"

/**
 * Обновляет ACL для файла с заданным индексом.
 * @param index Уникальный индекс файла.
 * @param new_user_id Новый идентификатор пользователя.
 * @param new_perms Новые права доступа.
 * @return 0 при успехе, -1 при ошибке.
 */
int myfs_update_acl(uint64_t index, uint32_t new_user_id, mode_t new_perms);

/**
 * Удаляет ACL-файл для файла с заданным индексом.
 * @param index Уникальный индекс файла.
 * @return 0 при успехе, -1 при ошибке.
 */
int remove_acl(uint64_t index);

#endif // ACL_UPDATE_H

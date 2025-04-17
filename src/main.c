#include <stdio.h>
#include <stdlib.h>
#include "scale_tests.h"
#include "btree.h"

// Глобальное B-дерево для метаданных
BTree global_tree = {0};

int main(int argc, char *argv[]) {
    int num_files = 100; // значение по умолчанию
    if (argc > 1) {
        num_files = atoi(argv[1]);
    }

    global_tree.root = malloc(sizeof(BTreeNode));
    if (!global_tree.root) {
        perror("malloc");
        return 1;
    }
    global_tree.root->num_keys = 0;
    global_tree.root->leaf = 1;

    printf("Запуск масштабных тестов...\n");
    run_scale_tests(num_files);

    // Здесь можно вызвать дополнительные тесты, если есть.

    // Освобождаем B-дерево (функция free_btree должна быть реализована)
    extern void free_btree(BTreeNode *node);
    free_btree(global_tree.root);

    return 0;
}

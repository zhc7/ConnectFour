//
// Created by zhc7 on 24-5-18.
//

#include "mem.h"

static Node pool[MAX_SIZE];
static int poolSize = 0;

static Node *freeStack[MAX_SIZE];
static int freeStackTop = 0;


Node *getNode() {
    Node *node = freeStackTop > 0 ? freeStack[--freeStackTop] : &pool[poolSize++];
    *node = Node();
    return node;
}

void freeNode(Node *node) {
    if (node != nullptr) {
        freeStack[freeStackTop++] = node;
        for (int i = 0; i < State::N; i++) {
            if (node->children[i] != nullptr) {
                freeNode(node->children[i]);
            }
        }
    }
}

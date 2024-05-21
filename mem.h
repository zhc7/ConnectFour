//
// Created by zhc7 on 24-5-21.
//

#ifndef MEM_H
#define MEM_H

#include "Node.h"

constexpr static int MAX_SIZE = 4e6;

Node *getNode();

void freeNode(Node *node);

#endif //MEM_H

//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_NODE_H
#define CONNECT4_NODE_H


#include "State.h"

class Node {
public:
    constexpr static float UCB_C = 1.0;
    static Node *root;

    State state;
    Node *children[12]{};
    float playerWins = 0;
    int visits = 0;
    bool isLeaf = true;
    float winRate = 0;
    float revSqrtVisit = 100;

    Node() = default;

    float ucbValue(float sqrtLogParentVisit) const;

    Node *select();

    int expand();

    void update(int winner);

    Node *pick(int y);

    ~Node() = default;

    int expandAction(int target);

protected:
    void handleMustWin(char winner);
};

#endif //CONNECT4_NODE_H

//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_NODE_H
#define CONNECT4_NODE_H


#include "State.h"

class Node {
public:
    constexpr static double UCB_C = 0.7;
    static Node *root;

    State state;
    Node *children[12]{};
    double playerWins = 0;
    int visits = 0;
    bool isLeaf = true;

    Node() = default;

    double ucbValue(double sqrtLogParentVisit) const;

    Node *select();

    Node *expand();

    void update(int winner);

    Node *pick(int y);

    ~Node();

protected:
    void handleMustWin(char winner);
};

#endif //CONNECT4_NODE_H

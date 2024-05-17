//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_NODE_H
#define CONNECT4_NODE_H


#include "State.h"

class Node {
public:
    constexpr const static double UCB_C = 0.7;
    static Node *root;

    State state;
    Node *children[12]{};
    int playerWins;
    int visits;
    bool isLeaf = true;

    explicit Node(const State& state);

    double ucbValue(int parentVisit) const;

    void handleMustWin(int winner);

    Node *select();

    Node *expand();

    void update(int isPlayerWin);

    Node *pick(int y);

    ~Node();
};

#endif //CONNECT4_NODE_H

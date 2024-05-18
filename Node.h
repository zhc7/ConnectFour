//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_NODE_H
#define CONNECT4_NODE_H


#include <unordered_map>

#include "State.h"


template<>
struct std::hash<State *> {
    size_t operator()(const State *state) const noexcept {
        size_t hash = 0;
        for (int i = 0; i < State::N; i++) {
            hash ^= state->top[i] << i;
        }
        for (int i = 0; i < 12; i++) {
            hash ^= state->board[i].row << (i << 1);
        }
        return hash;
    }
};

bool inline operator==(const State &lhs, const State &rhs) {
    for (int i = 0; i < State::N; i++) {
        if (lhs.top[i] != rhs.top[i]) {
            return false;
        }
    }
    for (int i = 0; i < 12; i++) {
        if (lhs.board[i].row != rhs.board[i].row) {
            return false;
        }
    }
    return true;
}


class Node {
public:
    constexpr static double UCB_C = 0.7;
    static Node *root;
    static std::unordered_map<State *, Node *> map;
    static std::unordered_map<Node *, int> rc;

    State state;
    Node *children[12]{};
    double playerWins = 0;
    int visits = 0;
    bool isLeaf = true;

    Node() = default;

    double ucbValue(double logParentVisit) const;

    Node *select();

    Node *expand();

    void update(int isPlayerWin);

    Node *pick(int y);

    ~Node();

protected:
    void handleMustWin(char winner);

    static inline void kill(Node *child);
};

#endif //CONNECT4_NODE_H
